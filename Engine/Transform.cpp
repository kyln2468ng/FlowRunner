#include "Transform.h"

Transform::Transform()
    :matTranslate_(XMMatrixIdentity()),
    matRotate_(XMMatrixIdentity()),
    matScale_(XMMatrixIdentity()),
    pParent_(nullptr)
{
    position_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
    rotate_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
    scale_ = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

Transform::~Transform()
{
}

void Transform::Calculation()
{
    matTranslate_= XMMatrixTranslation(position_.x, position_.y, position_.z);
    
    XMMATRIX r1, r2, r3;
    r1 = XMMatrixRotationZ(XMConvertToRadians(rotate_.z));
    r2 = XMMatrixRotationY(XMConvertToRadians(rotate_.y));
    r3 = XMMatrixRotationX(XMConvertToRadians(rotate_.x));
    matRotate_ = (r1 * r2 * r3);
    
    matScale_ = XMMatrixScaling(scale_.x, scale_.y, scale_.z);

}

XMMATRIX Transform::GetWorldMatrix()
{
    if (pParent_ != nullptr)
    {
        return matScale_ * matRotate_ * matTranslate_ * pParent_->GetWorldMatrix();
    }
    else
        return matScale_ * matRotate_ * matTranslate_; // ägèkÅñâÒì]Åñà⁄ìÆ
}

XMMATRIX Transform::GetNormalMatrix()
{
    return matRotate_ * XMMatrixInverse(nullptr, matScale_);
}

void Transform::SetVectorPosition(const XMVECTOR& vPos)
{
    XMStoreFloat3(&position_, vPos);
}

void Transform::SetVectorRotation(const XMVECTOR& vRot)
{
    XMStoreFloat3(&position_, vRot);
}

void Transform::SetVectroScale(const XMVECTOR& vScl)
{
    XMStoreFloat3(&position_, vScl);
}


