#include "pch.h"

#include <Ludus/Editor/Commands/Edit/Components.h>
#include <Ludus/Editor/Commands/Edit/Entities.h>
#include <Ludus/Editor/Commands/Edit/Selection.h>
#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Engine/Core/Variants.h>

namespace Ludus::Editor::Commands
{
	namespace
	{
		struct EditCommandVisitor
		{
			ProjectSessionCommandContext& Context;

			template<typename TComponent>
			void operator()(const EditCommand::AddComponent<TComponent>& command) const { Edit::Components::AddComponent(command, Context); }

			template<typename TComponent>
			void operator()(const EditCommand::RemoveComponent<TComponent>& command) const { Edit::Components::RemoveComponent(command, Context); }

			template<typename TComponent>
			void operator()(const EditCommand::UpdateComponent<TComponent>& command) const { Edit::Components::UpdateComponent(command, Context); }

			void operator()(const EditCommand::AddEntity& command) const { Edit::Entities::AddEntity(command, Context); }
			void operator()(const EditCommand::RemoveEntity& command) const { Edit::Entities::RemoveEntity(command, Context); }

			void operator()(const EditCommand::SelectEntity& command) const { Edit::Selection::SelectEntity(command, Context); }
			void operator()(const EditCommand::DeselectEntity& command) const { Edit::Selection::DeselectEntity(command, Context); }
			void operator()(const EditCommand::ClearSelection& command) const { Edit::Selection::ClearSelection(command, Context); }

			template<typename T>
			void operator()(T&& unhandled) const
			{
				Ludus::Engine::Core::Variants::Unhandled(unhandled);
			}
		};
	}

	void Execute(const EditCommand& command, ProjectSessionCommandContext& context)
	{
		std::visit(EditCommandVisitor { context }, command.Data);
	}
}
