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

        //乗算（回転合成）
        inline Quaternion& operator*=(const Quaternion& other)
        {
            quaternion_ = DirectX::XMQuaternionMultiply(quaternion_, other.quaternion_);
            return *this;
        }

        //正規化
        void Normalize()
        {
            quaternion_ = DirectX::XMQuaternionNormalize(quaternion_);
        }

        DirectX::XMMATRIX ToMatrix() const
        {
            return DirectX::XMMatrixRotationQuaternion(quaternion_);
        }
    };
    inline Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
    {
        Quaternion r = q1;
        r *= q2;
        return r;
    }
}

////必要なもの////
// Axis回転（軸回転）
// Quaternion乗算（回転の合成）ok
// Normalize    ok
// Matrix変換   ok
// Forword取得（キャラの向いてる向きを取る）
// sleap（回転補間）
// LookRotation（任意の方向にキャラを向ける。壁ジャンプとかで使用する）
////