#pragma once

#include <Ludus/UI/Flags/Flags.h>

namespace Ludus::UI::Context::TableContext
{
	void TableNextRow();

	void TableNextRowFirstColumn();

	void TableSetColumnIndex(int index);

	void TableSetupColumn(const char* label, Ludus::UI::Flags::Table flags = Ludus::UI::Flags::Table::None, float initWidthOrWeight = 0.0f);
}
