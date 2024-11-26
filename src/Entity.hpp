#pragma once
#include "Components.hpp"

#include <tuple>
// enum for entity types
enum class EntityType {
    Player,
    Enemy,
    SmallEnemy,
    Projectile,
    AbilityProjectile,
};


// Tuple of components for each entity
using ComponentTuple = std::tuple<CTransform,
    CShape, CCollison, CScore, CLifeSpan, CInput, CAbility>;


class Entity {
    ComponentTuple m_components;
    bool m_isActive = true; // is entity active
    EntityType m_type; // type of entity
    size_t m_id = 0; // unique id for each entity
    Entity(const EntityType& type, const size_t& id) : m_type(type), m_id(id) {} // constructor
    friend class EntityManager; // EntityManager is a friend class

public:
    bool IsActive() const {
        return m_isActive;
    }

    size_t GetID() const {
        return m_id;
    }

    EntityType GetType() const {
        return m_type;
    }

    void Destroy() {
        m_isActive = false;
    }
/**
 * @brief
 * @tparam T
 * @return check if the entity has the component
 */
    template<typename T>
    bool has() {
        return std::get<T>(m_components).exists;
    }

/**
 * @brief
 * @tparam T
 * @tparam TArgs
 * @param mArgs
 * add component to the entity with the given arguments if given
 * @return reference to the component
 */
    template<typename T, typename... TArgs>
    T& add(TArgs&&... mArgs) {
        auto& component = get<T>();
        component = T(std::forward<TArgs>(mArgs)...);
        component.exists = true;
        return component;
    }
/**
 * @brief
 * @tparam T
 * @return  reference to the component of the entity
 */
    template<typename T>
    T& get() {
        return std::get<T>(m_components);
    }

    // Const version of get
    template<typename T>
    const T& get() const {
        return std::get<T>(m_components);
    }
 /**
  * @brief
  * @tparam T
  * remove the component from the entity
  */
    template<typename T>
    void remove() {
        get<T>() = T();
    }
};
