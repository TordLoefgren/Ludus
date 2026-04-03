#pragma once

#include <memory>
#include <utility>

#include <Ludus/Engine/Runtime/ApplicationHost.h>
#include <Ludus/Engine/Runtime/ApplicationHostBuilder.h>
#include <Ludus/Engine/Runtime/SystemPhase.h>
#include <Ludus/Engine/Runtime/SystemPhaseOrder.h>
#include <Ludus/UI/Systems/ImGuiSystem.h>

namespace Ludus::UI::Systems
{
	inline void RegisterImGui(Ludus::Engine::Runtime::ApplicationHostBuilder& builder)
	{
		builder.Configure([](Ludus::Engine::Runtime::ApplicationHost& host)
		{
			auto imGuiSystem = std::make_unique<Ludus::UI::Systems::ImGuiSystem>(host.GetWindowHandle());

			host.AddSystem({
				{ Ludus::Engine::Runtime::SystemPhase::BeginFrame, Ludus::Engine::Runtime::SystemPhaseOrder::Before },
				{ Ludus::Engine::Runtime::SystemPhase::EndFrame, Ludus::Engine::Runtime::SystemPhaseOrder::After } },
				std::move(imGuiSystem));
		});
	}
}
