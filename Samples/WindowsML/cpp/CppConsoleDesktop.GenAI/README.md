# ONNX Runtime GenAI C Example

[Adapted from the GenAI examples here.](https://github.com/microsoft/onnxruntime-genai/tree/main/examples/c)

## Obtain a model

You can download a published model from Hugging Face or Foundry Local.

For example here are phi-3-mini-128k models in the foundry library.

```cmd

foundry model ls --filter alias=phi-3-mini-128k

Alias                          Device     Task               File Size    License      Model ID
-----------------------------------------------------------------------------------------------
phi-3-mini-128k                GPU        chat-completion    2.13 GB      MIT          Phi-3-mini-128k-instruct-cuda-gpu
                               GPU        chat-completion    2.13 GB      MIT          Phi-3-mini-128k-instruct-generic-gpu
                               CPU        chat-completion    2.54 GB      MIT          Phi-3-mini-128k-instruct-generic-cpu

```

Download a model, find the model cache folder, and build the path to the ONNX file.

```cmd
foundry model download Phi-3-mini-128k-instruct-generic-cpu

foundry cache location
%USERPROFILE%\.foundry\cache\models
```

Find the full path to the genai_config.json file.

```cmd

dir /s /b %USERPROFILE%\.foundry\cache\models\*genai_config.json

%USERPROFILE%\.foundry\cache\models\Microsoft\Phi-3.5-mini-instruct-generic-cpu\cpu-int4-rtn-block-32-acc-level-4\genai_config.json

```
Alternatively you can build a model yourself using the model builder. See [here](https://github.com/microsoft/onnxruntime-genai/blob/main/src/python/py/models/README.md) for more details.

## Run the model

1. Open [WindowsML-Samples.sln](../WindowsML-Samples.sln)

2. Set HelloPhi as startup project.

3. Set the debugger command line argument to point to your model

Example: `-m %USERPROFILE%\.foundry\cache\models\Microsoft\Phi-3.5-mini-instruct-generic-cpu\cpu-int4-rtn-block-32-acc-level-4`

Example - Run from the command line:

```cmd
dotnet run -- -m %USERPROFILE%\.foundry\cache\models\Microsoft\Phi-3.5-mini-instruct-generic-cpu\cpu-int4-rtn-block-32-acc-level-4
```
