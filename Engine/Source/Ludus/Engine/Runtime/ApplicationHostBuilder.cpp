#include "pch.h"

#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include <Ludus/Engine/Core/ExecutionFlags.h>
#include <Ludus/Engine/Core/Mask.h>
#include <Ludus/Engine/Graphics/RenderingSystem2D.h>
#include <Ludus/Engine/Graphics/RenderViewSystem.h>
#include <Ludus/Engine/Physics/Core/PhysicsSystem2D.h>
#include <Ludus/Engine/Runtime/ApplicationHost.h>
#include <Ludus/Engine/Runtime/ApplicationHostBuilder.h>
#include <Ludus/Engine/Runtime/SystemConstraints.h>
#include <Ludus/Engine/Runtime/SystemPhaseOrder.h>
#include <Ludus/Engine/Scripting/ScriptSystem.h>

namespace Ludus::Engine::Runtime
{
	std::unique_ptr<Ludus::Engine::Runtime::ApplicationHost> ApplicationHostBuilder::Build()
	{
		auto host = ApplicationHost::Create(
			m_RuntimeOptions,
			m_WindowOptions
		);

		for (auto& command : m_BuilderCommands)
		{
			command(*host);
		}

		return host;
	}

	ApplicationHostBuilder& ApplicationHostBuilder::Configure(ApplicationHostBuilderCommand command)
	{
		m_BuilderCommands.emplace_back(std::move(command));
		return *this;
	}

	ApplicationHostBuilder& ApplicationHostBuilder::WithRuntimeOptions(const Ludus::Engine::Runtime::RuntimeOptions runtimeOptions)
	{
		m_RuntimeOptions = runtimeOptions;
		return *this;
	}

	ApplicationHostBuilder& ApplicationHostBuilder::WithWindowOptions(const Ludus::Engine::Windowing::WindowOptions windowOptions)
	{
		m_WindowOptions = windowOptions;
		return *this;
	}
}
