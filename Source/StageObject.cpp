#include "StageObject.h"


StageObject::StageObject(GameObject* parent)
    :GameObject(parent,"StageObject"),filename_(nullptr)
{
}

StageObject::StageObject(const std::string& filename, const XMVECTOR& position, const XMVECTOR& rotation, const XMVECTOR& scale)
{
    filename_ = filename;
    transform_.SetVectorPosition(position);
    transform_.SetVectorRotation(rotation);
    transform_.SetVectroScale(scale);
}

StageObject::~StageObject()
{
}

void StageObject::Initialize()
{
    hModel_ = Model::Load(filename_);
    assert(hModel_ >= 0);

    transform_.Calculation();
    Model::SetTransform(hModel_, transform_);
}

bool StageObject::CollideLine(RayCastData& data)
{
    Model::RayCast(hModel_, data);
    return data.isHit;
}
