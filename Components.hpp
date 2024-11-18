#pragma once
#include "SFML/Graphics.hpp"
#include <SFML/System/Vector2.hpp>
class Component
{
public:
	bool exists = false;
};

class CTransform : public Component
{
public:
	sf::Vector2f position = { 0,0 };
	sf::Vector2f valocity = { 0,0 };
	float rotation = 0;
	CTransform() = default;
	CTransform(sf::Vector2f pos, sf::Vector2f val, float rot)
		: position(pos), valocity(val), rotation(rot) {}
};

class CShape : public Component
{
public:
	sf::CircleShape circle;
	CShape() = default;
	CShape(float radius, size_t Npoints, const sf::Color& fillcolor, sf::Color& outline, float lineThick) :
		circle(radius, Npoints) {
		circle.setFillColor(fillcolor);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(lineThick);
		circle.setOrigin(radius, radius);
	}

};
class CCollison : public Component
{
public:
	float radius = 0;
	CCollison() = default;
	CCollison(float r)
		: radius(r) {}

};

class CScore : public Component
{
public:
	int score = 0;
	CScore() = default;
	CScore(int sc)
		: score(sc) {}
};

class CLifeSpan : public Component
{
 public:
	int lifeTime = 0;
	int RemainingLifeTime = 0;
	CLifeSpan() = default;
	CLifeSpan(int life)
		: lifeTime(life), RemainingLifeTime(life) {}
};

class CInput : public Component
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool shoot = false;
	bool ability = false;
	CInput() = default;
	
};

class CAbility : public Component
{};