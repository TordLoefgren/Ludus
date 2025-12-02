#pragma once

#include <Ludus/Core/ISystem.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/Shape.h>
#include <Ludus/Physics/Core/BodyType.h>

namespace Ludus::Editor::Core
{
	class Scene : public Ludus::Core::ISystem
	{
	public:
		Scene() = default;

		virtual void OnAttachImpl() override;
		virtual void UpdateImpl(float deltaTime) override;
	};
}
