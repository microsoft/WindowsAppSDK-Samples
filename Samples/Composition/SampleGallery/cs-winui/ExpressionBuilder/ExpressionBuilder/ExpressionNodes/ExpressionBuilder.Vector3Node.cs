///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (c) Microsoft Corporation.  All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------

namespace ExpressionBuilder
{
    using System.Numerics;

// Ignore warning: 'Vector3Node' defines operator == or operator != but does not override Object.Equals(object o) && Object.GetHashCode()
#pragma warning disable CS0660, CS0661
    public sealed class Vector3Node : ExpressionNode
    {
        internal Vector3Node() 
        {
        }
        
        internal Vector3Node(Vector3 value)
        {
            _value = value;
            _nodeType = ExpressionNodeType.ConstantValue;
        }
        
        internal Vector3Node(string paramName)
        {
            _paramName = paramName;
            _nodeType = ExpressionNodeType.ConstantParameter;
        }
        
        internal Vector3Node(string paramName, Vector3 value)
        {
            _paramName = paramName;
            _value = value;
            _nodeType = ExpressionNodeType.ConstantParameter;

            SetVector3Parameter(paramName, value);
        }
        
        
        //
        // Operator overloads
        //

        public static implicit operator Vector3Node(Vector3 value) { return new Vector3Node(value); }

        public static Vector3Node operator +(Vector3Node left, Vector3Node right) { return ExpressionFunctions.Function<Vector3Node>(ExpressionNodeType.Add, left, right);      }
        public static Vector3Node operator -(Vector3Node left, Vector3Node right) { return ExpressionFunctions.Function<Vector3Node>(ExpressionNodeType.Subtract, left, right); }
        public static Vector3Node operator -(Vector3Node value)                   { return ExpressionFunctions.Function<Vector3Node>(ExpressionNodeType.Negate, value);         }

        public static Vector3Node operator *(Vector3Node left, ScalarNode right)  { return ExpressionFunctions.Function<Vector3Node>(ExpressionNodeType.Multiply, left, right); }
        public static Vector3Node operator *(Vector3Node left, Vector3Node right) { return ExpressionFunctions.Function<Vector3Node>(ExpressionNodeType.Multiply, left, right); }

        public static Vector3Node operator /(Vector3Node left, Vector3Node right) { return ExpressionFunctions.Function<Vector3Node>(ExpressionNodeType.Divide, left, right);  }
        public static Vector3Node operator %(Vector3Node left, Vector3Node right) { return ExpressionFunctions.Function<Vector3Node>(ExpressionNodeType.Modulus, left, right); }

        public static BooleanNode operator ==(Vector3Node left, Vector3Node right) { return ExpressionFunctions.Function<BooleanNode>(ExpressionNodeType.Equals, left, right);    }
        public static BooleanNode operator !=(Vector3Node left, Vector3Node right) { return ExpressionFunctions.Function<BooleanNode>(ExpressionNodeType.NotEquals, left, right); }

        
        //
        // Subchannels
        //
        
        public enum Subchannel
        {
            X,
            Y,
            Z
        }
        
        // Commonly accessed subchannels
        public ScalarNode  X  { get { return GetSubchannels(Subchannel.X);               } }
        public ScalarNode  Y  { get { return GetSubchannels(Subchannel.Y);               } }
        public ScalarNode  Z  { get { return GetSubchannels(Subchannel.Z);               } }
        public Vector2Node XY { get { return GetSubchannels(Subchannel.X, Subchannel.Y); } }

        /// <summary> Create a new type by re-arranging the Vector subchannels. </summary>
        public ScalarNode  GetSubchannels(Subchannel s) { return SubchannelsInternal<ScalarNode>(s.ToString()); }
        public Vector2Node GetSubchannels(Subchannel s1, Subchannel s2) { return SubchannelsInternal<Vector2Node>(s1.ToString(), s2.ToString()); }
        public Vector3Node GetSubchannels(Subchannel s1, Subchannel s2, Subchannel s3) { return SubchannelsInternal<Vector3Node>(s1.ToString(), s2.ToString(), s3.ToString()); }
        public Vector4Node GetSubchannels(Subchannel s1, Subchannel s2, Subchannel s3, Subchannel s4) { return SubchannelsInternal<Vector4Node>(s1.ToString(), s2.ToString(), s3.ToString(), s4.ToString()); }

        public Matrix3x2Node GetSubchannels(Subchannel s1, Subchannel s2, Subchannel s3, Subchannel s4, Subchannel s5, Subchannel s6)
        { 
            return SubchannelsInternal<Matrix3x2Node>(s1.ToString(), s2.ToString(), s3.ToString(), s4.ToString(), s5.ToString(), s6.ToString()); 
        }

        public Matrix4x4Node GetSubchannels(Subchannel s1, Subchannel s2, Subchannel s3, Subchannel s4,
                                         Subchannel s5, Subchannel s6, Subchannel s7, Subchannel s8,
                                         Subchannel s9, Subchannel s10, Subchannel s11, Subchannel s12,
                                         Subchannel s13, Subchannel s14, Subchannel s15, Subchannel s16)
        {
            return SubchannelsInternal<Matrix4x4Node>(s1.ToString(), s2.ToString(), s3.ToString(), s4.ToString(),
                                                      s5.ToString(), s6.ToString(), s7.ToString(), s8.ToString(),
                                                      s9.ToString(), s10.ToString(), s11.ToString(), s12.ToString(),
                                                      s13.ToString(), s14.ToString(), s15.ToString(), s16.ToString());
        }

        internal protected override string GetValue()
        {
            return $"Vector3({_value.X},{_value.Y},{_value.Z})";
        }

        private Vector3 _value;
    }
#pragma warning restore CS0660, CS0661
}