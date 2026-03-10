#pragma once
#include <DirectXMath.h>

namespace math
{
    struct Quaternion
    {
        DirectX::XMVECTOR quaternion_;

        Quaternion()
        {
            quaternion_ = DirectX::XMQuaternionIdentity();
        }

        DirectX::XMMATRIX ToMatrix() const
        {
            return DirectX::XMMatrixRotationQuaternion(quaternion_);
        }
    };
}