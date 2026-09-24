# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

import os
from pathlib import Path
import shutil
import tempfile

from PIL import Image
import numpy as np
import onnxruntime as ort
from winml import WinML


def _get_ort_compatibility_api(name: str):
    api = getattr(ort, name, None)
    if api is not None:
        return api

    from onnxruntime.capi import onnxruntime_pybind11_state

    return getattr(onnxruntime_pybind11_state, name)


def _device_type_name(device) -> str:
    return str(device.device.type).rsplit(".", 1)[-1].upper()


def _is_discrete_gpu(device) -> bool:
    return (
        _device_type_name(device) == "GPU"
        and device.device.metadata.get("Discrete") == "1"
    )


def _matches_ep_vendor(device) -> bool:
    return device.device.vendor == device.ep_vendor


def _is_default_cpu_ep(device) -> bool:
    return _device_type_name(device) == "CPU" and device.ep_vendor == "Microsoft"


def _policy_device_sort_key(device):
    device_type_priority = {"NPU": 0, "GPU": 1, "CPU": 2}
    device_type = _device_type_name(device)
    return (
        device_type_priority.get(device_type, 2),
        0 if _is_discrete_gpu(device) else 1,
        0 if _matches_ep_vendor(device) else 1,
        1 if _is_default_cpu_ep(device) else 0,
        device.ep_name,
    )


def _select_policy_device_groups(ep_devices, policy):
    ordered_devices = sorted(ep_devices, key=_policy_device_sort_key)
    selected_devices = []
    if not ordered_devices:
        return {}

    policy_name = getattr(policy, "name", str(policy).rsplit(".", 1)[-1])
    if policy_name in {
        "PREFER_NPU",
        "MAX_EFFICIENCY",
        "MIN_OVERALL_POWER",
    }:
        if _device_type_name(ordered_devices[0]) == "NPU":
            selected_devices.append(ordered_devices[0])
    elif policy_name in {"PREFER_GPU", "MAX_PERFORMANCE"}:
        first_gpu = next(
            (device for device in ordered_devices if _device_type_name(device) == "GPU"),
            None,
        )
        if first_gpu is not None:
            selected_devices.append(first_gpu)

    first_cpu = next(
        (device for device in ordered_devices if _device_type_name(device) == "CPU"),
        None,
    )
    if first_cpu is not None:
        selected_devices.append(first_cpu)
        if not _is_default_cpu_ep(first_cpu) and _is_default_cpu_ep(
            ordered_devices[-1]
        ):
            selected_devices.append(ordered_devices[-1])

    selected_groups = {}
    for device in selected_devices:
        selected_groups.setdefault(device.ep_name, []).append(device)
    return selected_groups


def _compile_model_to_temporary_file(
    session_options: ort.SessionOptions,
    model_path: Path,
    compiled_model_path: Path,
) -> Path | None:
    temporary_path = None
    temporary_directory = None

    try:
        temporary_directory = Path(tempfile.mkdtemp(
            prefix=f"{compiled_model_path.stem}.",
            dir=compiled_model_path.parent,
        ))
        temporary_path = temporary_directory / compiled_model_path.name

        model_compiler = ort.ModelCompiler(
            session_options,
            model_path,
            embed_compiled_data_into_model=True,
            external_initializers_file_path=None,
        )
        model_compiler.compile_to_file(str(temporary_path))
        if not temporary_path.is_file() or temporary_path.stat().st_size == 0:
            raise RuntimeError("Compilation did not produce a model.")

        generated_paths = list(temporary_directory.iterdir())
        if generated_paths != [temporary_path]:
            generated_names = ", ".join(path.name for path in generated_paths)
            raise RuntimeError(
                "Compilation produced unexpected external artifacts: "
                f"{generated_names}"
            )

        return temporary_path
    except Exception as error:
        print(f"Model compilation failed: {error}")
        print("Using the original model for this run.")
        if temporary_directory is not None:
            try:
                shutil.rmtree(temporary_directory)
            except OSError as cleanup_error:
                print(
                    "Could not remove temporary compilation directory: "
                    f"{cleanup_error}"
                )
        return None


def _remove_temporary_model(temporary_path: Path | None) -> None:
    if temporary_path is not None:
        temporary_directory = temporary_path.parent
        try:
            temporary_path.unlink(missing_ok=True)
            temporary_directory.rmdir()
        except OSError as error:
            print(f"Could not remove temporary compilation output: {error}")


def load_and_preprocess_image(image_path: Path) -> np.ndarray:
    img = Image.open(image_path)    
    if img.mode != 'RGB':
        img = img.convert('RGB')
    img = img.resize((224, 224))
    means = np.array([0.485, 0.456, 0.406]).reshape(1, 1, 3)
    stds = np.array([0.229, 0.224, 0.225]).reshape(1, 1, 3)
    img_array = np.array(img).astype(np.float32)
    img_array = (img_array - means) / stds    
    img_array = img_array.transpose((2, 0, 1))
    img_array = np.expand_dims(img_array, axis=0)
    return img_array.astype(np.float32)

def load_labels(label_file: Path) -> list[str]:
    with open(label_file, 'r') as f:
        labels = [line.strip().split(',')[1] for line in f.readlines()]
    return labels

def print_results(labels : list[str], results: np.ndarray, is_logit=False) -> None:
    def softmax(x):
        exp_x = np.exp(x - np.max(x))
        return exp_x / exp_x.sum()
    results = results.flatten()
    if is_logit:
        results = softmax(results)
    top_k = 5
    top_indices = np.argsort(results)[-top_k:][::-1]
    print("Top Predictions:")
    print("-"*50)
    print(f"{'Label':<32} {'Confidence':>10}")
    print("-"*50)
    
    for i in top_indices:
        print(f"{labels[i]:<32} {results[i]*100:>10.2f}%")
    
    print("-"*50)

if __name__ == "__main__":
    print("Registering execution providers ...")
    registered_eps = WinML().register_execution_providers_to_ort()
    print(f"Registered execution providers: {registered_eps}")

    resource_path = Path(__file__).parent.parent 

    print("Setting session options ...")
    session_options = ort.SessionOptions()
    # Change your policy here.
    ep_policy = ort.OrtExecutionProviderDevicePolicy.PREFER_NPU
    session_options.set_provider_selection_policy(ep_policy)

    print("Compiling model ...")
    model_path = resource_path / "Model" / "SqueezeNet.onnx"
    compiled_model_path = resource_path / "Model" / "SqueezeNet_ctx.onnx"

    relevant_device_groups = {}
    try:
        get_ep_devices = _get_ort_compatibility_api("get_ep_devices")
        get_compatibility_info_from_model = _get_ort_compatibility_api(
            "get_compatibility_info_from_model"
        )
        get_model_compatibility_for_ep_devices = _get_ort_compatibility_api(
            "get_model_compatibility_for_ep_devices"
        )
        compiled_model_compatibility = _get_ort_compatibility_api(
            "OrtCompiledModelCompatibility"
        )

        relevant_device_groups = _select_policy_device_groups(
            get_ep_devices(), ep_policy
        )
        policy_name = getattr(
            ep_policy, "name", str(ep_policy).rsplit(".", 1)[-1]
        )
        print(
            f"{policy_name} compatibility check selected "
            "the following EP group(s): "
            + ", ".join(relevant_device_groups)
        )
    except Exception as error:
        print(f"Could not prepare compiled model compatibility checks: {error}")

    use_compiled_model = False
    temporary_model_path = None
    candidate_model_path = (
        compiled_model_path if compiled_model_path.exists() else None
    )
    candidate_is_temporary = False

    if candidate_model_path is None and relevant_device_groups:
        temporary_model_path = _compile_model_to_temporary_file(
            session_options, model_path, compiled_model_path
        )
        candidate_model_path = temporary_model_path
        candidate_is_temporary = True
    elif candidate_model_path is None:
        print(
            "Skipping model compilation because compatibility could not be "
            "evaluated."
        )

    try:
        while candidate_model_path is not None:
            print(f"Checking compiled model compatibility: {candidate_model_path}")
            candidate_is_optimal = False

            if not relevant_device_groups:
                print("No relevant execution provider devices were found.")
            else:
                matching_metadata_found = False
                preferred_group_metadata_missing = False
                all_matching_groups_optimal = True

                for group_index, (ep_name, ep_devices) in enumerate(
                    relevant_device_groups.items()
                ):
                    device_descriptions = ", ".join(
                        f"{ep_device.device.type} "
                        f"(EP vendor={ep_device.ep_vendor})"
                        for ep_device in ep_devices
                    )
                    print(
                        f"Probing EP '{ep_name}' with device(s): "
                        f"{device_descriptions}"
                    )

                    try:
                        compatibility_info = get_compatibility_info_from_model(
                            str(candidate_model_path), ep_name
                        )
                        if compatibility_info is None:
                            print(
                                "Compiled model has no compatibility metadata for "
                                f"{ep_name}."
                            )
                            # Selection preserves the policy-preferred EP as the first group.
                            if group_index == 0:
                                preferred_group_metadata_missing = True
                            continue

                        matching_metadata_found = True
                        status = get_model_compatibility_for_ep_devices(
                            ep_devices, compatibility_info
                        )
                        status_name = getattr(
                            status, "name", str(status).rsplit(".", 1)[-1]
                        )
                        print(
                            f"Compiled model compatibility for {ep_name}: "
                            f"{status_name}"
                        )
                        if (
                            status
                            != compiled_model_compatibility.EP_SUPPORTED_OPTIMAL
                        ):
                            all_matching_groups_optimal = False
                    except Exception as error:
                        all_matching_groups_optimal = False
                        print(
                            f"Could not check the compiled model for {ep_name}: "
                            f"{error}"
                        )

                if not matching_metadata_found:
                    print(
                        "Compiled model has no compatibility metadata for any "
                        "selected EP group."
                    )
                elif preferred_group_metadata_missing:
                    print(
                        "Compiled model has no compatibility metadata for the "
                        "preferred EP group."
                    )

                candidate_is_optimal = (
                    matching_metadata_found
                    and not preferred_group_metadata_missing
                    and all_matching_groups_optimal
                )

            if candidate_is_optimal:
                if candidate_is_temporary:
                    try:
                        os.replace(candidate_model_path, compiled_model_path)
                        print(f"Compiled model saved to {compiled_model_path}.")
                        _remove_temporary_model(candidate_model_path)
                        temporary_model_path = None
                    except OSError as error:
                        print(
                            f"Could not replace compiled model cache "
                            f"'{compiled_model_path}' with temporary output "
                            f"'{candidate_model_path}': {error}"
                        )
                        break

                use_compiled_model = True
                break

            if candidate_is_temporary:
                print(
                    "Newly compiled model was not reported as "
                    "EP_SUPPORTED_OPTIMAL."
                )
                break

            if not relevant_device_groups:
                print(
                    "Skipping recompilation because compatibility could not be "
                    "evaluated."
                )
                break

            print("Existing compiled model is not optimal; recompiling.")
            temporary_model_path = _compile_model_to_temporary_file(
                session_options, model_path, compiled_model_path
            )
            candidate_model_path = temporary_model_path
            candidate_is_temporary = True
    finally:
        _remove_temporary_model(temporary_model_path)

    if use_compiled_model:
        print("Using compiled model.")
        model_path_to_use = compiled_model_path
    else:
        print("No usable compile output. Using original model.")
        model_path_to_use = model_path

    print("Creating session ...")
    session = ort.InferenceSession(
        model_path_to_use,
        sess_options=session_options
    )

    labels = load_labels(resource_path / "Model" / "SqueezeNet.Labels.txt")

    images_folder = resource_path / "Images"
    for image_file in images_folder.iterdir():  
        print(f"Running inference on image: {image_file}")
        print("Preparing input ...")
        img_array = load_and_preprocess_image(image_file)
        print("Running inference ...")
        input_name = session.get_inputs()[0].name
        results = session.run(None, {input_name: img_array})[0]
        if not isinstance(results, np.ndarray):
            raise TypeError("Unexpected output type from model.")
        print_results(labels, results, is_logit=False)
