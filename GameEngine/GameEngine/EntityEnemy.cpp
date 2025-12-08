#include "EntityEnemy.h"

#include "ResourceManager.h"

EntityEnemy::~EntityEnemy()
{
    ResourceManager::Instance().Unload("27471003-7f27-42e3-a791-c159b33f5198");
    ResourceManager::Instance().Unload("");
}

bool EntityEnemy::Init()
{
    _mesh = (MeshResource *)ResourceManager::Instance().Load("27471003-7f27-42e3-a791-c159b33f5198");
    _texture = (TextureResource *)ResourceManager::Instance().Load("");
    return true;
}

bool EntityEnemy::Update()
{
    // Do something... like update _health
    return true;
}
