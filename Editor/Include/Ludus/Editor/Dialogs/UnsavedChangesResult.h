#pragma once

#include <cstdint>

namespace Ludus::Editor::Dialogs
{
	enum class UnsavedChangesResult : uint8_t
	{
		Save,
		DontSave,
		Cancel
	};
}
