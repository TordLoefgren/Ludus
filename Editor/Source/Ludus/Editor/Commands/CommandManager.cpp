#include "pch.h"

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <utility>

#include <Ludus/Editor/Commands/CommandManager.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Editor::Commands
{
	void CommandManager::EnqueueCommands(Ludus::Editor::Commands::CommandSet&& commands)
	{
		PendingCommands.EditCommands.insert(
			PendingCommands.EditCommands.end(),
			std::make_move_iterator(commands.EditCommands.begin()),
			std::make_move_iterator(commands.EditCommands.end())
		);

		PendingCommands.RequestCommands.insert(
			PendingCommands.RequestCommands.end(),
			std::make_move_iterator(commands.RequestCommands.begin()),
			std::make_move_iterator(commands.RequestCommands.end())
		);

		PendingCommands.UICommands.insert(
			PendingCommands.UICommands.end(),
			std::make_move_iterator(commands.UICommands.begin()),
			std::make_move_iterator(commands.UICommands.end())
		);
	}

	void CommandManager::AddEditCommand(Ludus::Editor::Commands::EditCommand command)
	{
		PendingCommands.EditCommands.emplace_back(std::move(command));
	}

	void CommandManager::AddRequestCommand(Ludus::Editor::Commands::RequestCommand command)
	{
		PendingCommands.RequestCommands.emplace_back(std::move(command));
	}

	void CommandManager::AddUICommand(Ludus::Editor::Commands::UICommand command)
	{
		PendingCommands.UICommands.emplace_back(std::move(command));
	}

	Ludus::Engine::Core::EntityHandle CommandManager::ResolveEntity(const EntityReference& reference) const
	{
		return std::visit([&](auto&& value) -> Ludus::Engine::Core::EntityHandle
		{
			using Alt = std::decay_t<decltype(value)>;

			if constexpr (std::is_same_v<Alt, Ludus::Engine::Core::EntityHandle>)
			{
				return value;
			}
			else
			{
				auto iter = m_TempToBinding.find(value.Temp);
				LUDUS_ASSERT(iter != m_TempToBinding.end(), "Unresolved temporary entity reference.");

				return iter->second.Handle;
			}
		}, reference.Value);
	}

	void CommandManager::BindEntityReference(TempReference temp, Ludus::Engine::Core::EntityHandle handle)
	{
		m_TempToBinding[temp] = TempEntityBinding { .Handle = handle, .LastSeenFrame = m_FrameIndex };
	}

	void CommandManager::ClearEntityReferences()
	{
		// Keep the temp entity bindings alive for one extra frame so deferred command chains can still be resolved.
		const auto previousFrameIndex = m_FrameIndex > 0 ? m_FrameIndex - 1 : 0;

		std::erase_if(m_TempToBinding, [&](const auto& pair)
		{
			return pair.second.LastSeenFrame < previousFrameIndex;
		});

		m_FrameIndex++;
	}
}
