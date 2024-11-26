#pragma once
#include "Entity.hpp"
#include <memory>
#include <map>
#include <vector>
#include <algorithm>

using EntityVec = std::vector<std::shared_ptr<Entity>>;

class EntityManager
{
    EntityVec m_entities;
    EntityVec m_entitiesToAdd;
    std::map<EntityType, EntityVec> m_entityMap;
    size_t m_totalEntities = 0;
/**
 * @brief
 * @param EntityVec
 * Destroy and remove inactive entities from the vector of entities
 */
    void DestroyInactiveEntities(EntityVec& vec) {
        auto iter = std::remove_if(vec.begin(), vec.end(), [](const std::shared_ptr<Entity>& entity) {
            return !entity->IsActive();
            });
        vec.erase(iter, vec.end());
    }

public:
    /**
     * @brief
     * Update the vector of entities add new entities to the vector and remove inactive entities
     */
    void Update() {
        for (auto& entity : m_entitiesToAdd) {
            m_entities.push_back(entity);
            //add to the map
            if (m_entityMap.find(entity->GetType()) == m_entityMap.end()) // if 
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
/**
 * @brief
 * @param EntityType
 * add entity of the given type to the vector of entities to be added
 * @return  shared pointer to the new entity
 */
    std::shared_ptr<Entity> AddEntity(const EntityType& type) {
        // Fixed: Use make_shared correctly by passing constructor arguments directly
        auto entity = std::shared_ptr<Entity>(new Entity(type, m_totalEntities++));
        m_entitiesToAdd.push_back(entity);
        return entity;
    }
/**
 * @brief
 * @return vector of entities
 */

    const EntityVec& getEntities() {
        return m_entities;
    }
/**
 * @brief
 * @param type
 * @return vector of entities of the given type
 */
    const EntityVec& getEntities(const EntityType& type) {
        return m_entityMap[type];
    }
};