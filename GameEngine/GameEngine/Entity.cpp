#include "Entity.h"

void Entity::AddGUID(std::string guid)
{
	if (guid.length() == 36) {
		_GUIDs.push_back(guid);
	}
}

Transform *Entity::GetTransform()
{
	return &_transform;
}

std::vector<std::string> Entity::GetGUIDs()
{
	return _GUIDs;
}
