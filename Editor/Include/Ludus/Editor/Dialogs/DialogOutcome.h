#pragma once

#include <utility>
#include <variant>

namespace Ludus::Editor::Dialogs
{
	template<typename T>
	struct DialogOutcome
	{
		struct None { };
		struct Cancelled { };
		struct Confirmed { T Payload; };

		using Variant = std::variant <None, Cancelled, Confirmed>;

		Variant Data;

		static DialogOutcome NoneState() { return DialogOutcome { None { } }; }
		static DialogOutcome Cancel() { return DialogOutcome { Cancelled { } }; }
		static DialogOutcome Confirm(T payload) { return DialogOutcome { Confirmed { std::move(payload) } }; }
	};
}
