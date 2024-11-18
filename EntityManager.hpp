#pragma once
#include "Entity.hpp"
using EntityVec = std::vector<std::shared_ptr<Entity>>;

class EntityManager
{
    EntityVec m_entities;
    EntityVec m_entitiesToAdd;
    std::map<EntityType, EntityVec> m_entityMap;
    size_t m_totalEntities = 0;

    void DestroyInactiveEntities(EntityVec& vec) {
        auto iter = std::remove_if(vec.begin(), vec.end(), [](const std::shared_ptr<Entity>& entity) {
            return !entity->IsActive();
            });
        vec.erase(iter, vec.end());
    }

public:
    void Update() {
        for (auto& entity : m_entitiesToAdd) {
            m_entities.push_back(entity);
            //add to the map
            if (m_entityMap.find(entity->GetType()) == m_entityMap.end())
            {
                m_entityMap[entity->GetType()] = EntityVec();
            }
            m_entityMap[entity->GetType()].push_back(entity);
        }
        DestroyInactiveEntities(m_entities);
        //remove from the vectors in the map
        for (auto& pair : m_entityMap) {
            DestroyInactiveEntities(pair.second);
        }
        m_entitiesToAdd.clear();
    }

    std::shared_ptr<Entity> AddEntity(const EntityType& type) {
        // Fixed: Use make_shared correctly by passing constructor arguments directly
        auto entity = std::shared_ptr<Entity>(new Entity(type, m_totalEntities++));
        m_entitiesToAdd.push_back(entity);
        return entity;
    }

    const EntityVec& getEntities() {
        return m_entities;
    }

    const EntityVec& getEntities(const EntityType& type) {
        return m_entityMap[type];
    }
};