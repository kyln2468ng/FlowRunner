#include "Camera.h"
#include "Input.h"
#include "Quaternion.h"

//変数
XMVECTOR position_;	//カメラの位置（視点）
XMVECTOR target_;	//見る位置（焦点）
XMMATRIX viewMatrix_;	//ビュー行列
XMMATRIX projMatrix_;	//プロジェクション行列
float yaw_;
float pitch_;

namespace {
    static float distance_ = -8.0f;
    static float height_ = 5.0f;
}

//初期化
void Camera::Initialize()
{
    position_ = XMVectorSet(0, 3, -10, 0);	//カメラの位置
    target_ = XMVectorSet(0, 0, 0, 0);	//カメラの焦点

    //プロジェクション行列
    projMatrix_ = XMMatrixPerspectiveFovLH(XM_PIDIV4, (FLOAT)800 / (FLOAT)600, 0.1f, 100.0f); //ズームさせたいときとかはUpdateに書く
}

//更新
void Camera::Update()
{
    //Input::SetMousePosition(LOWORD(lParam), HIWORD(lParam));
    XMVECTOR moveVec = Input::GetMouseDelta();

    constexpr float DegToRad = XM_PI / 180.0f; //constexprのままにするかは一旦仮

    yaw_ += XMVectorGetX(moveVec) * 0.005;
    pitch_ -= XMVectorGetY(moveVec) * 0.005;

    if (pitch_ >= 40.0f * DegToRad) {
        pitch_ = 40.0f * DegToRad;
    }
    if (pitch_ <= -40.0 * DegToRad) {
        pitch_ = -40.0f * DegToRad;
    }

    math::Quaternion yawRot = math::Quaternion::AxisAngle(XMVectorSet(0, 1, 0, 0), yaw_);
    XMVECTOR right = yawRot.Right();
    math::Quaternion pitchRot = math::Quaternion::AxisAngle(right, pitch_);
    math::Quaternion rot = yawRot* pitchRot;

    XMVECTOR offset = XMVectorSet(0.0f, height_, distance_, 0.0f);
    offset = XMVector3Rotate(offset, rot.quaternion_);

    //position_ = target_ + XMVectorSet(0.0f, height_, 0.0f, 0.0f) + offset;
    

    XMVECTOR desiredPosition = target_ + offset;
    position_ = XMVectorLerp(position_, desiredPosition, 0.1f);
    
    //ビュー行列の作成
    viewMatrix_ = XMMatrixLookAtLH(position_, target_, XMVectorSet(0, 1, 0, 0));


}

//位置を設定
void Camera::SetPosition(XMVECTOR position)
{
    position_ = position;
}

void Camera::SetPosition(XMFLOAT3 position)
{
    position_ = XMLoadFloat3(&position);
}

//焦点を設定
void Camera::SetTarget(XMVECTOR target)
{
    target_ = target;
}

void Camera::SetTarget(XMFLOAT3 target)
{
    target_ = XMLoadFloat3(&target);
}

//ビュー行列を取得
XMMATRIX Camera::GetViewMatrix()
{
    return viewMatrix_;
}

//プロジェクション行列を取得
XMMATRIX Camera::GetProjectionMatrix()
{
    return projMatrix_;
}

XMFLOAT3 Camera::GetPosition()
{
    XMFLOAT3 pos = { 0,0,0 };
    XMStoreFloat3(&pos, position_);
    return pos;
}

XMFLOAT3 Camera::GetTarget()
{
    XMFLOAT3 tage = { 0,0,0 };
    XMStoreFloat3(&tage, target_);
    return tage;
}
