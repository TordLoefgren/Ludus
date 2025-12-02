#pragma once

#include <Ludus/Core/ISystem.h>

namespace Ludus::Editor::Core
{
	class PanelSystem : public Ludus::Core::ISystem
	{
	protected:
		virtual void UpdateImpl(float deltaTime) override { DrawPanel(); }

		virtual void DrawPanel() = 0;
	};
}
