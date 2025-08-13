// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

using Microsoft.ML.OnnxRuntime;
using Microsoft.ML.OnnxRuntime.Tensors;
using System.Runtime.InteropServices.WindowsRuntime;

namespace WindowsML.Shared
{
    /// <summary>
    /// Inference execution functionality
    /// </summary>
    public static class InferenceEngine
    {
        /// <summary>
        /// Prepare input tensor and run inference
        /// </summary>
        public static IDisposableReadOnlyCollection<DisposableNamedOnnxValue> RunInference(
            InferenceSession session,
            DenseTensor<float> input)
        {
            Console.WriteLine("Preparing input ...");

            // Prepare input tensor
            string inputName = session.InputMetadata.First().Key;
            DenseTensor<float> inputTensor = new(
                input.ToArray(),
                [1, 3, 224, 224], // Shape of the tensor
                false             // isReversedStride should be explicitly set to false
            );

            // Bind inputs and run inference
            List<NamedOnnxValue> inputs = [NamedOnnxValue.CreateFromTensor(inputName, inputTensor)];

            Console.WriteLine("Running inference ...");
            return session.Run(inputs);
        }

        /// <summary>
        /// Extract output tensor from inference results
        /// </summary>
        public static float[] ExtractResults(InferenceSession session, IDisposableReadOnlyCollection<DisposableNamedOnnxValue> results)
        {
            // Extract output tensor
            string outputName = session.OutputMetadata.First().Key;
            return results.First(r => r.Name == outputName).AsEnumerable<float>().ToArray();
        }
    }
}
