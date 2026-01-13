#pragma once

#include <list>
#include <string>
#include "Transform.h"

using std::string;
using std::list;

class SphereCollider;

class GameObject
{
public:
	GameObject();
	GameObject(GameObject* parent, const string& name);
	virtual ~GameObject();

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Release() = 0;
	void UpdateSub();
	void DrawSub();
	void ReleaseSub();

	void SetPosition(XMFLOAT3 position);
	void SetPosition(float x, float y, float z);

	//bool isDie();
	void KillMe();

	GameObject* GetRootJob();
	GameObject* FindChildObject(const string& name);
	GameObject* FindObject(const string& name);
	const std::string& GetObjectName() const;

	void AddCollider(SphereCollider* pCollider);
	void Collision(GameObject* pTarget);
	virtual void OnCollision(GameObject* pTarget) {};
	void RoundRobin(GameObject* pTarget);

	template<class T> GameObject*Instantiate(GameObject* parent)
	{
		T* obj = new T(parent);
		obj->Initialize();
		childList_.push_back(obj);
		return(obj);
	}
protected:
	list<GameObject *>	childList_;
	Transform			transform_;
	GameObject*			pParent_;
	string				objectName_;
	SphereCollider*		pCollider_;
private:
	bool isDead_;
};

