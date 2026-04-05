#pragma once

#include <string_view>

#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Core/Id.h>
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

			writer.Emit(Token::Key { "Id" });
			writer.Emit(Token::Uint { script.Id.Value });

			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<Script, SerializationException> Deserialize(
			ITokenStreamReader& reader,
			Ludus::Engine::Core::EntityId ownerId
		)
		{
			try
			{
				Script script;
				script.OwnerId = ownerId;
				bool hasId = false;

				Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
				{
					if (key == "Id")
					{
						script.Id = { Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader) };
						if (!script.Id.IsValid())
						{
							throw SerializationException("Invalid script id.");
						}
						hasId = true;
						return;
					}

					Ludus::Engine::Serialization::Core::SkipValue(reader);
				});

				if (!hasId)
				{
					throw SerializationException("No script id found.");
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
