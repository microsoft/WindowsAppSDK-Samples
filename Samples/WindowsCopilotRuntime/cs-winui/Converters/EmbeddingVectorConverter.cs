// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.UI.Xaml.Data;
using Microsoft.Windows.AI.Foundation;
using Microsoft.Windows.AI.Text;
using System;
using System.Text;

namespace WindowsCopilotRuntimeSample.Converters;

internal partial class EmbeddingVectorConverter : IValueConverter
{
    public object Convert(object value, Type targetType, object parameter, string language)
    {
        if (value is LanguageModelEmbeddingVectorResult languageModelEmbeddingVectorResult)
        {
            var embeddings = languageModelEmbeddingVectorResult.EmbeddingVectors;
            StringBuilder sb = new();
            for (var index = 0; index < embeddings.Count; ++index)
            {
                EmbeddingVector embeddingVector = embeddings[index];
                sb.Append('[');
                var values = new float[embeddingVector.Size];
                embeddingVector.GetValues(values);
                sb.Append(string.Join(", ", values));
                if (sb.Length > 1024)
                {
                    sb.Append("...]");
                    break;
                }
                sb.Append(']');
            }

            var embeddingsTex = sb.ToString();
            return embeddingsTex;
        }

        return value;
    }

    public object ConvertBack(object value, Type targetType, object parameter, string language)
    {
        throw new NotImplementedException();
    }
}