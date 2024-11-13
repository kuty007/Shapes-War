#pragma once
#include "Entity.hpp"
using EntityVec = std::vector<std::shared_ptr<Entity>>;




class EntityManager
{
	EntityVec m_entities;
	EntityVec m_entitiesToAdd;
	std::map<EntityType,EntityVec> m_entityMap;
	size_t m_totalEntities = 0;
	void DestroyInactiveEntities(EntityVec& vec) {
		auto iter = std::remove_if(vec.begin(), vec.end(), [](const std::shared_ptr<Entity>& entity) {
			return !entity->IsActive();
			});
		vec.erase(iter, vec.end());
		
	}
public:
		void Update() {
		for (auto& entity : m_entities) {
			//entity->Update();
		}
		for (auto& entity : m_entitiesToAdd) {
			m_entities.push_back(entity);
		}
		DestroyInactiveEntities(m_entities);
		//remove from the vectors in the map
		for (auto& pair : m_entityMap) {
			DestroyInactiveEntities(pair.second);
		}
		m_entitiesToAdd.clear();
	}

		std::shared_ptr<Entity> AddEntity(const EntityType& type) {
			auto entity = std::make_shared<Entity>(type, m_totalEntities++);
			m_entitiesToAdd.push_back(entity);
			//add to the map
			if (m_entityMap.find(type) == m_entityMap.end())
			{
				m_entityMap[type] = EntityVec();
			}
			m_entityMap[type].push_back(entity);
			return entity;
		}
				
};