#pragma once
#include "Components.hpp"

#include <tuple>
//create Enum for all the entities
enum class EntityType {
	Player,
	Enemy,
	SmallEnemy,
	Projectile,
	AbilityProjectile,
};



using ComponentTuple = std::tuple<CTransform, 
	CShape, CCollison, CScore, CLifeSpan,CInput>;


class Entity {
    ComponentTuple m_components;
    bool m_isActive = true;
    EntityType m_type;
    size_t m_id = 0;
    Entity(const EntityType& type, const size_t& id) : m_type(type), m_id(id) {}

    friend class EntityManager;

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

    template<typename T>
	bool has() {
		return std::get<T>(m_components).exists;
	}


    template<typename T, typename... TArgs>
    T& add(TArgs&&... mArgs) {
        auto& component = get<T>();
        component = T(std::forward<TArgs>(mArgs)...);
        component.exists = true;
        return component;
    }

    template<typename T>
    T& get() {
        return std::get<T>(m_components);
    }

    // Const version of get
    template<typename T>
    const T& get() const {
        return std::get<T>(m_components);
    }

    template<typename T>
    void remove() {
        get<T>() = T();
    }
};
