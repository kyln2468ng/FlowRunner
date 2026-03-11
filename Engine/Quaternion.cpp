#include "Quaternion.h"

using namespace DirectX;

math::Quaternion math::Quaternion::LookRotation(DirectX::XMVECTOR dir, DirectX::XMVECTOR up)
{
    XMVECTOR forward = XMVector3Normalize(dir);
    XMVECTOR upNorm = XMVector3Normalize(up);

    float dot = XMVectorGetX(XMVector3Dot(forward, upNorm));

    if (fabs(dot) > 0.999f)
    {
        upNorm = DirectX::XMVectorSet(0, 0, 1, 0);
    }

    XMVECTOR right = XMVector3Normalize(XMVector3Cross(upNorm, forward));

    XMVECTOR verticalUp = XMVector3Cross(forward, right);

    DirectX::XMMATRIX rot = { right,verticalUp,forward,DirectX::XMVectorSet(0,0,0,1) };

    return Quaternion(XMQuaternionRotationMatrix(rot));
}
