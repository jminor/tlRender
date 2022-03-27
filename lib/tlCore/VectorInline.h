// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

namespace tl
{
    namespace math
    {
        template<>
        inline Vector2<int>::Vector2() noexcept :
            x(0),
            y(0)
        {}

        template<>
        inline Vector2<float>::Vector2() noexcept :
            x(0.F),
            y(0.F)
        {}

        template<typename T>
        inline Vector2<T>::Vector2(T x, T y) noexcept :
            x(x),
            y(y)
        {}

        template<>
        inline Vector3<float>::Vector3() noexcept :
            x(0.F),
            y(0.F),
            z(0.F)
        {}

        template<typename T>
        inline Vector3<T>::Vector3(T x, T y, T z) noexcept :
            x(x),
            y(y),
            z(z)
        {}

        template<typename T>
        constexpr Vector2<T> operator + (const Vector2<T>& value, T t)
        {
            return Vector2<T>(value.x + t, value.y + y);
        }

        template<typename T>
        constexpr Vector3<T> operator + (const Vector3<T>& value, T t)
        {
            return Vector3<T>(value.x + t, value.y + y, value.z + z);
        }

        template<typename T>
        constexpr Vector4<T> operator + (const Vector4<T>& value, T t)
        {
            return Vector4<T>(value.x + t, value.y + y, value.z + z, value.w + w);
        }

        template<typename T>
        constexpr Vector2<T> operator - (const Vector2<T>& value, T t)
        {
            return Vector2<T>(value.x - t, value.y - t);
        }

        template<typename T>
        constexpr Vector3<T> operator - (const Vector3<T>& value, T t)
        {
            return Vector3<T>(value.x - t, value.y - t, value.z - t);
        }

        template<typename T>
        constexpr Vector4<T> operator - (const Vector4<T>& value, T t)
        {
            return Vector4<T>(value.x - t, value.y - t, value.z - t, value.w - t);
        }

        template<typename T>
        constexpr Vector2<T> operator * (const Vector2<T>& value, T t)
        {
            return Vector2<T>(value.x * t, value.y * t);
        }

        template<typename T>
        constexpr Vector3<T> operator * (const Vector3<T>& value, T t)
        {
            return Vector3<T>(value.x * t, value.y * t, value.z * t);
        }

        template<typename T>
        constexpr Vector4<T> operator * (const Vector4<T>& value, T t)
        {
            return Vector4<T>(value.x * t, value.y * t, value.z * t, value.w * t);
        }

        template<typename T>
        constexpr Vector2<T> operator / (const Vector2<T>& value, T t)
        {
            return Vector2<T>(value.x / t, value.y / t);
        }

        template<typename T>
        constexpr Vector3<T> operator / (const Vector3<T>& value, T t)
        {
            return Vector3<T>(value.x / t, value.y / t, value.z / t);
        }

        template<typename T>
        constexpr Vector4<T> operator / (const Vector4<T>& value, T t)
        {
            return Vector4<T>(value.x / t, value.y / t, value.z / t, value.w / t);
        }

        template<typename T>
        inline bool Vector2<T>::operator == (const Vector2<T>& other) const
        {
            return x == other.x && y == other.y;
        }

        template<typename T>
        bool Vector2<T>::operator != (const Vector2<T>& other) const
        {
            return !(*this == other);
        }

        template<typename T>
        inline bool Vector3<T>::operator == (const Vector3<T>& other) const
        {
            return x == other.x && y == other.y && z == other.z;
        }

        template<typename T>
        inline bool Vector3<T>::operator != (const Vector3<T>& other) const
        {
            return !(*this == other);
        }

        template<typename T>
        inline bool Vector4<T>::operator == (const Vector4<T>& other) const
        {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }

        template<typename T>
        inline bool Vector4<T>::operator != (const Vector4<T>& other) const
        {
            return !(*this == other);
        }
    };
}
