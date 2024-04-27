///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (c) Microsoft Corporation.  All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------

namespace ExpressionBuilder
{
    ///---------------------------------------------------------------------------------------------------------------------
    /// 
    /// class ExpressionValues.CurrentValue
    ///    ToDo: Add description after docs written
    /// 
    ///---------------------------------------------------------------------------------------------------------------------

    // ExpressionValues is a static class instead of a namespace to improve intellisense discoverablity and consistency with the other helper classes.
    public static partial class ExpressionValues
    {        
        /// <summary> Refer to the current value of the property this expression is connected to. </summary>
        public static class CurrentValue
        {
            /// <summary> Create a reference to the current value of the boolean property that this expression will be connected to. </summary>
            public static BooleanNode    CreateBooleanCurrentValue()    { return ExpressionNode.CreateValueKeyword<BooleanNode>(ValueKeywordKind.CurrentValue);    }
            
            /// <summary> Create a reference to the current value of the float property that this expression will be connected to. </summary>
            public static ScalarNode     CreateScalarCurrentValue()     { return ExpressionNode.CreateValueKeyword<ScalarNode>(ValueKeywordKind.CurrentValue);     }
            
            /// <summary> Create a reference to the current value of the Vector2 property that this expression will be connected to. </summary>
            public static Vector2Node    CreateVector2CurrentValue()    { return ExpressionNode.CreateValueKeyword<Vector2Node>(ValueKeywordKind.CurrentValue);    }
            
            /// <summary> Create a reference to the current value of the Vector3 property that this expression will be connected to. </summary>
            public static Vector3Node    CreateVector3CurrentValue()    { return ExpressionNode.CreateValueKeyword<Vector3Node>(ValueKeywordKind.CurrentValue);    }
            
            /// <summary> Create a reference to the current value of the Vector4 property that this expression will be connected to. </summary>
            public static Vector4Node    CreateVector4CurrentValue()    { return ExpressionNode.CreateValueKeyword<Vector4Node>(ValueKeywordKind.CurrentValue);    }
            
            /// <summary> Create a reference to the current value of the Color property that this expression will be connected to. </summary>
            public static ColorNode      CreateColorCurrentValue()      { return ExpressionNode.CreateValueKeyword<ColorNode>(ValueKeywordKind.CurrentValue);      }
            
            /// <summary> Create a reference to the current value of the Quaternion property that this expression will be connected to. </summary>
            public static QuaternionNode CreateQuaternionCurrentValue() { return ExpressionNode.CreateValueKeyword<QuaternionNode>(ValueKeywordKind.CurrentValue); }
            
            /// <summary> Create a reference to the current value of the Matrix3x2 property that this expression will be connected to. </summary>
            public static Matrix3x2Node  CreateMatrix3x2CurrentValue()  { return ExpressionNode.CreateValueKeyword<Matrix3x2Node>(ValueKeywordKind.CurrentValue);  }
            
            /// <summary> Create a reference to the current value of the Matrix4x4 property that this expression will be connected to. </summary>
            public static Matrix4x4Node  CreateMatrix4x4CurrentValue()  { return ExpressionNode.CreateValueKeyword<Matrix4x4Node>(ValueKeywordKind.CurrentValue);  }
        }
    }
}