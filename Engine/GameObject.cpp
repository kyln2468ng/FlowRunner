#include "GameObject.h"
#include "SphereCollider.h"
#include "Windows.h"

GameObject::GameObject()
	:pParent_(nullptr)
{
}

GameObject::GameObject(GameObject* parent, const string& name)
	:pParent_(parent), objectName_(name), isDead_(false)
{
	if (parent != nullptr)
	{
		transform_.pParent_ = &(parent->transform_);
	}
}

GameObject::~GameObject()
{
}

void GameObject::UpdateSub()
{
	transform_.Calculation();
	this->Update();

	RoundRobin(GetRootJob());

	for (auto child : childList_)
	{
		child->UpdateSub();
	}

	for (auto itr = childList_.begin();itr != childList_.end();)
	{
		if ((*itr)->isDead_)
		{
			(*itr)->ReleaseSub();
			delete (*itr);
			itr = childList_.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

void GameObject::DrawSub()
{
	//0.自分を描画
	Draw(); // this->Draw(); どちらにするかは場所ごとで違う
	//1.子オブジェクトを描画 childList_の各要素に対してDrawを呼ぶ
	/*for (auto it = childList_.begin();it != childList_.end(); ++it)
	{
		(*it)->DrawSub();
	}*/

	for (auto child : childList_)
	{
		child->DrawSub();
	}
}

void GameObject::ReleaseSub()
{
	for (auto child : childList_)
	{
		child->ReleaseSub();
	}
}

void GameObject::SetPosition(XMFLOAT3 position)
{
	transform_.position_ = position;
}

void GameObject::SetPosition(float x, float y, float z)
{
	SetPosition(XMFLOAT3(x, y, z));
}

//bool GameObject::isDie()
//{
//	return isDead_;
//}

void GameObject::KillMe()
{
	isDead_ = true;
}


GameObject* GameObject::GetRootJob()
{
	if (pParent_ == nullptr)
	{
		return this;
	}
	else
	{
		return pParent_->GetRootJob();
	}
}

GameObject* GameObject::FindChildObject(const string& name)
{
	if (this->objectName_ == name)
	{
		return this; //自分が探されていた
	}
	else
	{
		for (auto child : childList_)
		{
			GameObject* result = child->FindChildObject(name);
			if (result != nullptr)
			{
				return result; //見つかった
			}
		}
		return nullptr; // 見つからなかった
	}
}

GameObject* GameObject::FindObject(const string& name)
{
	GameObject* rootJob = GetRootJob();
	GameObject* result = rootJob->FindChildObject(name);
	return result;
}

const std::string& GameObject::GetObjectName(void) const
{
	return objectName_;
}

void GameObject::AddCollider(SphereCollider* pCollider)
{
	pCollider_ = pCollider;
}

void GameObject::Collision(GameObject* pTarget)
{
	// this->pCollider_とpTarget_->はぶつかっているか
	// 閾値=お互いの半径＋半径
	float thisR = this->pCollider_->GetRadius();
	float tgtR = pTarget->pCollider_->GetRadius();
	float thre = (thisR + tgtR) * (thisR + tgtR);

	// 2つのコライダーの距離計算をする
	XMFLOAT3 thisP = this->transform_.position_;
	XMFLOAT3 tgtP = pTarget->transform_.position_;
	float dist = (thisP.x - tgtP.x) * (thisP.x - tgtP.x) +
				 (thisP.y - tgtP.y) * (thisP.y - tgtP.y) +
				 (thisP.z - tgtP.z) * (thisP.z - tgtP.z);
	// コライダー同士が交差していたら
	if (dist <= thre)
	{
		// なんかする
		//MessageBoxA(0, "ぶつかった", "Collider", MB_OK);

		this->OnCollision(pTarget);
	}
}

void GameObject::RoundRobin(GameObject* pTarget)
{
	// ①自分にコライダーがなかったらreturn
	if (pCollider_ == nullptr)
		return;

	// ②自分とターゲット自体のコライダーの当たり判定
	if (pTarget->pCollider_ != nullptr && this->pCollider_ != pTarget->pCollider_)
		Collision(pTarget);

	// ③再帰的なやつでターゲットの子オブジェクトの当たり判定してく
	for (auto itr : pTarget->childList_)
		RoundRobin(itr);
}
