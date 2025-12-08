#pragma once
#include "Entity.h"

class EntityEnemy : public Entity
{
private:
	float _health = 100.0f;

public:
	EntityEnemy() = default;
	~EntityEnemy();

	bool Init() override;

	bool Update();
};

