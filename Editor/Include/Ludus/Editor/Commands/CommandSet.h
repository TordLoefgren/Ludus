#pragma once

#include <vector>

#include <Ludus/Editor/Commands/EditCommand.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/UICommand.h>

namespace Ludus::Editor::Commands
{
	struct CommandSet
	{
		std::vector<EditCommand> EditCommands;
		std::vector<RequestCommand> RequestCommands;
		std::vector<UICommand> UICommands;

		bool Empty() const
		{
			return EditCommands.empty() &&
				RequestCommands.empty() &&
				UICommands.empty();
		}
	};
}
