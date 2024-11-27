// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

struct Matrix2x2
{
    Matrix2x2(float m11, float m12, float m21, float m22) noexcept :
        m11(m11), m12(m12), m21(m21), m22(m22)
    {
    }

    // Default ctor creates an identity transform.
    Matrix2x2() noexcept : Matrix2x2(1, 0, 0, 1)
    {
    }

    // Construct a Matrix2x2 from a float4x4, float3x2, or D2D1_MATRIX_3x2F
    template<class T>
    explicit Matrix2x2(T const& matrix) noexcept :
        Matrix2x2(matrix.m11, matrix.m12, matrix.m21, matrix.m22)
    {
    }

    auto To4x4() const noexcept
    {
        return winrt::Windows::Foundation::Numerics::float4x4(
            m11, m12, 0, 0,
            m21, m22, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1);
    }

    auto ToD2D() const noexcept
    {
        return D2D1::Matrix3x2F(m11, m12, m21, m22, 0, 0);
    }

    bool IsInvertible() const noexcept
    {
        return ToD2D().IsInvertible();
    }

    auto GetInverse() const noexcept
    {
        auto matrix = ToD2D();
        matrix.Invert();
        return Matrix2x2(matrix);
    }

    bool operator==(Matrix2x2 const& rhs) const noexcept
    {
        return m11 == rhs.m11 && m12 == rhs.m12 && m21 == rhs.m21 && m22 == rhs.m22;
    }

    float m11, m12;
    float m21, m22;
};
