#pragma once

#include <cstdint>
#include <unordered_map>

#include <Ludus/Editor/Commands/CommandSet.h>
#include <Ludus/Editor/Commands/EntityReference.h>
#include <Ludus/Engine/Core/Entity.h>

namespace Ludus::Editor::Commands
{
	struct CommandManager
	{
	private:
		struct TempEntityBinding
		{
			Ludus::Engine::Core::EntityHandle Handle {};
			std::uint64_t LastSeenFrame = 0;
		};

		std::unordered_map<TempReference, TempEntityBinding> m_TempToBinding;
		std::uint64_t m_FrameIndex = 0;

	public:
		Ludus::Editor::Commands::CommandSet PendingCommands;

		void EnqueueCommands(Ludus::Editor::Commands::CommandSet&& commands);

		void AddEditCommand(Ludus::Editor::Commands::EditCommand command);
		void AddRequestCommand(Ludus::Editor::Commands::RequestCommand command);
		void AddUICommand(Ludus::Editor::Commands::UICommand command);

		Ludus::Engine::Core::EntityHandle ResolveEntity(const EntityReference& reference) const;
		void BindEntityReference(TempReference temp, Ludus::Engine::Core::EntityHandle handle);
		void ClearEntityReferences();
	};
}
