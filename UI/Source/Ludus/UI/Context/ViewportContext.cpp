#include "pch.h"

#include <Ludus/UI/Context/ViewportContext.h>
#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Types/Viewport.h>

namespace Ludus::UI::Context::ViewportContext
{
	Ludus::UI::Types::Viewport GetMainViewport()
	{
		const auto* mainViewport = ImGui::GetMainViewport();

		Ludus::UI::Types::Viewport viewport {
			.Id = mainViewport->ID,
			.Flags = static_cast<Ludus::UI::Flags::Viewport>(mainViewport->Flags),
			.Position = { mainViewport->Pos.x, mainViewport->Pos.y },
			.Size = { mainViewport->Size.x, mainViewport->Size.y },
			.FramebufferScale = { mainViewport->FramebufferScale.x, mainViewport->FramebufferScale.y },
			.WorkPosition = { mainViewport->WorkPos.x, mainViewport->WorkPos.y },
			.WorkSize = { mainViewport->WorkSize.x, mainViewport->WorkSize.y },
			.DpiScale = mainViewport->DpiScale
		};

		return viewport;
	}
}
