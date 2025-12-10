#pragma once
#include "Entity.h"

class EntityMushroom : public Entity
{
private:


public:
	EntityMushroom() = default;
	~EntityMushroom();

	bool Init() override;

};

