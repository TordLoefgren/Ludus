#include "pch.h"

#include <Ludus/UI/Context/TableContext.h>
#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Context::TableContext
{
	void TableNextRow() { ImGui::TableNextRow(); }

	void TableNextRowFirstColumn()
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
	}

	void TableSetColumnIndex(int index) { ImGui::TableSetColumnIndex(index); }

	void TableSetupColumn(const char* label, Ludus::UI::Flags::TableColumn flags, float initWidthOrWeight)
	{
		ImGui::TableSetupColumn(label, static_cast<int>(flags), initWidthOrWeight);
	}
}
