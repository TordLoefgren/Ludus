#pragma once

#include <Ludus/Core/ISystem.h>

namespace Ludus::Engine
{
	class SceneSystem2D : public Ludus::Core::ISystem
	{
	public:
		SceneSystem2D() = default;
		virtual ~SceneSystem2D() = default;

		virtual void FixedUpdate(float fixedTime) override { };

		virtual void Update(float deltaTime) override { };
	};
}
