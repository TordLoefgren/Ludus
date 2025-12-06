#pragma once

#include <Ludus/Engine/Core/ISystem.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/Engine/Physics/Core/BodyType.h>

namespace Ludus::Editor::Core
{
	class Scene : public Ludus::Engine::Core::ISystem
	{
	public:
		Scene() = default;

		virtual void OnAttachImpl() override;
		virtual void UpdateImpl(float deltaTime) override;
	};
}
