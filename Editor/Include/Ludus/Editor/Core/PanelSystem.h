#pragma once

#include <Ludus/Engine/Core/ISystem.h>

namespace Ludus::Editor::Core
{
	class PanelSystem : public Ludus::Engine::Core::ISystem
	{
	protected:
		virtual void UpdateImpl(float deltaTime) override { DrawPanel(); }

		virtual void DrawPanel() = 0;
	};
}
