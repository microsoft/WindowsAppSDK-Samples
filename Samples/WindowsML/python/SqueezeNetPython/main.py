# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License. See LICENSE.md in the repo root for license information.

from pathlib import Path
from PIL import Image
import numpy as np
import onnxruntime as ort
import subprocess
import sys
import json

def register_execution_providers():
    # Run this in another process to avoid a issue where 
    # the pywinrt module stops the tensorRT execution provider from loading.
    worker_script = str(Path(__file__).parent / 'winml_worker.py')
    result = subprocess.check_output([sys.executable, worker_script], text=True)
    paths = json.loads(result)
    for item in paths.items():
        ort.register_execution_provider_library(item[0], item[1])
        print(f"Registered execution provider: {item[0]} with library path: {item[1]}")

def load_and_preprocess_image(image_path):
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

def load_labels(label_file):
    with open(label_file, 'r') as f:
        labels = [line.strip().split(',')[1] for line in f.readlines()]
    return labels

def print_results(labels, results, is_logit=False):
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
    register_execution_providers()
    
    print("Creating session ...")

    resource_path = Path(__file__).parent.parent 
    model_path = resource_path / "Model" / "SqueezeNet.onnx"
    compiled_model_path = resource_path / "Model" / "SqueezeNet_ctx.onnx"
    session_options = ort.SessionOptions()
    # Change your policy here.
    session_options.set_provider_selection_policy(ort.OrtExecutionProviderDevicePolicy.PREFER_NPU)
    assert session_options.has_providers()

    if compiled_model_path.exists():
        print("Using compiled model")
    else:
        print("No compiled model found, attempting to create compiled model at ", compiled_model_path)  
        model_compiler = ort.ModelCompiler(session_options, model_path)
        print("Starting compile, this may take a few moments..." )
        try:
            model_compiler.compile_to_file(compiled_model_path)
            print("Model compiled successfully")
        except Exception as e:
            print("Model compilation failed:", e)
            print("Falling back to uncompiled model")

    model_path_to_use = compiled_model_path if compiled_model_path.exists() else model_path

    session = ort.InferenceSession(
        model_path_to_use,
        sess_options=session_options,
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
        print_results(labels, results, is_logit=False)
