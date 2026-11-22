#pragma once
#include <vector>
#include "Engine/Core/Vertex.hpp"
#include "Engine/Physics/Body.hpp"

class Player : public Body
{
public:
	Player();
	void Update(float deltaSeconds) override;
	void Render() const;

private:
	std::vector<Vertex> m_localVerts;
};
