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

        Quaternion(const DirectX::XMVECTOR q)
        {
            quaternion_ = q;
        }

        Quaternion(float x, float y, float z, float w)
        {
            quaternion_ = DirectX::XMVectorSet(x, y, z, w);
        }

        /// <summary>
        /// 乗算（回転の合成）
        /// </summary>
        /// <param name="other">合成する回転</param>
        /// <returns>合成後のQuaternion</returns>
        inline Quaternion& operator*=(const Quaternion& other)
        {
            quaternion_ = DirectX::XMQuaternionMultiply(quaternion_, other.quaternion_);
            return *this;
        }

        /// <summary>
        /// 正規化をする
        /// </summary>
        void Normalize()
        {
            quaternion_ = DirectX::XMQuaternionNormalize(quaternion_);
        }

        /// <summary>
        /// 共役Quaternionの取得(x,y,zの符号の反転）
        /// </summary>
        /// <returns>共役Quaternion</returns>
        Quaternion Conjugate() const
        {
            return Quaternion(DirectX::XMQuaternionConjugate(quaternion_));
        }

        /// <summary>
        /// 逆回転Quaternionの取得
        /// </summary>
        /// <returns>逆回転Quaternion</returns>
        Quaternion Inverse() const
        {
            return Quaternion(DirectX::XMQuaternionInverse(quaternion_));
        }

        /// <summary>
        /// Quaternionを回転行列に変換
        /// </summary>
        /// <returns>回転行列</returns>
        DirectX::XMMATRIX ToMatrix() const
        {
            return DirectX::XMMatrixRotationQuaternion(quaternion_);
        }

        /// <summary>
        /// 軸と角度からQuaternionを生成
        /// </summary>
        /// <param name="axis">回転ベクトル</param>
        /// <param name="angle">回転角度（ラジアン）</param>
        /// <returns>回転Quaternion</returns>
        static Quaternion AxisAngle(DirectX::XMVECTOR axis,float angle) 
        {
            Quaternion result;
            axis = DirectX::XMVector3Normalize(axis);
            result.quaternion_ = DirectX::XMQuaternionRotationAxis(axis, angle);
            return result;
        }

        /// <summary>
        /// 指定方向を向く回転Quaternionを生成
        /// </summary>
        /// <param name="dir">向きたい方向ベクトル</param>
        /// <param name="up">上方向ベクトル</param>
        /// <returns>回転Quaternion</returns>
        static Quaternion LookRotation(DirectX::XMVECTOR dir, DirectX::XMVECTOR up);

        /// <summary>
        /// Quaternionの球面線形補間
        /// </summary>
        /// <param name="q1">開始回転</param>
        /// <param name="q2">終了回転</param>
        /// <param name="lerp">補間係数（0～1）</param>
        /// <returns>補間後の回転</returns>
        static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float lerp)
        {
            return Quaternion(DirectX::XMQuaternionSlerp(q1.quaternion_, q2.quaternion_, lerp));
        }

        /// <summary>
        /// 前方向ベクトルの取得
        /// </summary>
        /// <returns>Z+方向への回転後ベクトル</returns>
        DirectX::XMVECTOR Forward() const
        {
            return DirectX::XMVector3Rotate(DirectX::XMVectorSet(0, 0, 1, 0), quaternion_);
        }

        /// <summary>
        /// 上方向ベクトルの取得
        /// </summary>
        /// <returns>Y+方向への回転後ベクトル</returns>
        DirectX::XMVECTOR Up() const
        {
            return DirectX::XMVector3Rotate(DirectX::XMVectorSet(0, 1, 0, 0), quaternion_);
        }

        /// <summary>
        /// 右方向ベクトルの取得
        /// </summary>
        /// <returns>X+方向への回転後ベクトル</returns>
        DirectX::XMVECTOR Right() const
        {
            return DirectX::XMVector3Rotate(DirectX::XMVectorSet(1, 0, 0, 0), quaternion_);
        }

    };
    inline Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
    {
        Quaternion r = q1;
        r *= q2;
        return r;
    }
}