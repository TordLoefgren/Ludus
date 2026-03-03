#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>
#include <Ludus/Engine/Serialization/Core/TokenRead.h>

namespace Ludus::Engine::Serialization::Schemas
{
	using Script = Ludus::Engine::Components::ScriptComponent;
	using ITokenStreamWriter = Ludus::Engine::Serialization::Core::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;
	using SerializationException = Ludus::Engine::Serialization::Core::SerializationException;
	using Token = Ludus::Engine::Serialization::Core::Token;

	struct ScriptComponentSchema
	{
		inline static void Serialize(ITokenStreamWriter& writer, const Script& script)
		{
			writer.Emit(Token::StartObject { });

			writer.Emit(Token::Key { "OwnerHandle" });
			writer.Emit(Token::Uint { script.OwnerHandle });

			writer.Emit(Token::Key { "Handle" });
			writer.Emit(Token::Uint { script.Handle });

			writer.Emit(Token::Key { "Name" });
			writer.Emit(Token::String { script.Name });

			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<Script, SerializationException> Deserialize(ITokenStreamReader& reader)
		{
			try
			{
				Script script;
				bool hasOwner = false;
				bool hasHandle = false;

				Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
				{
					if (key == "OwnerHandle")
					{
						script.OwnerHandle = Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader);
						hasOwner = true;
						return;
					}
					if (key == "Handle")
					{
						script.Handle = Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader);
						hasHandle = true;
						return;
					}
					if (key == "Name")
					{
						script.Name = std::string(Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data);
						return;
					}

					Ludus::Engine::Serialization::Core::SkipValue(reader);
				});

				if (!hasOwner)
				{
					throw SerializationException("No owner handle found.");
				}

				if (!hasHandle)
				{
					throw SerializationException("No script handle found.");
				}

				return script;
			}
			catch (const SerializationException& ex)
			{
				const auto error = Ludus::Engine::Serialization::Core::WithContext(
					ex, "ScriptComponentSchema::Deserialize"
				);

				return Ludus::Engine::Core::Unexpected<SerializationException>::Create(error);
			}
		}
	};
}
