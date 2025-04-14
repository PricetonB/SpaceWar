#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Entity.h"
#include <memory>
#include <algorithm>
#include <map>

using EntityPtr = std::shared_ptr<EntityCls>;
using EntityVec = std::vector<EntityPtr>;
using EntityMap = std::map<std::string, EntityVec>;


class EntityManagerCls
{
private:
	EntityVec Entities;
	EntityMap EntitiesMap;
	EntityVec ToAddEntity;
	size_t TotalEntities = 0;

	void RemoveDeadEntities(EntityVec& entityVec);
public:
	EntityManagerCls();
	void Update();
	EntityPtr AddEntity(const std::string & tag);
	EntityVec& GetEntities();
	EntityVec& GetEntities(const std::string & tag);
	EntityMap& GetEntityMap();

};

