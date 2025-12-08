#include "EntityEnemy.h"

#include "ResourceManager.h"
#include "Resource.h"

EntityEnemy::~EntityEnemy()
{
    ResourceManager::Instance().UnloadResource("27471003-7f27-42e3-a791-c159b33f5198");
    ResourceManager::Instance().UnloadResource("");
}

bool EntityEnemy::Init()
{
    if (!ResourceManager::Instance().LoadResource("27471003-7f27-42e3-a791-c159b33f5198", _mesh)) {
        std::cerr << "EntityEnemy::Init(): Failed to load mesh" << std::endl;
        return false;
    }
    if (!ResourceManager::Instance().LoadResource("", _texture)) {
        std::cerr << "EntityEnemy::Init(): Failed to load texture" << std::endl;
        return false;
    }

    _mesh->Init();
    _texture->Init();

    return true;
}

bool EntityEnemy::Update()
{
    // Do something... like update _health
    return true;
}
