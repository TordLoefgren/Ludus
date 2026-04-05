#pragma once

#include <Ludus/Engine/Components/DisplayNameComponent.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>
#include <Ludus/Engine/Serialization/Core/TokenRead.h>

namespace Ludus::Engine::Serialization::Schemas
{
	using DisplayName = Ludus::Engine::Components::DisplayNameComponent;
	using ITokenStreamWriter = Ludus::Engine::Serialization::Core::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;
	using SerializationException = Ludus::Engine::Serialization::Core::SerializationException;
	using Token = Ludus::Engine::Serialization::Core::Token;

	struct DisplayNameComponentSchema
	{
		inline static void Serialize(ITokenStreamWriter& writer, const DisplayName& displayName)
		{
			writer.Emit(Token::StartObject { });

			writer.Emit(Token::Key { "Name" });
			writer.Emit(Token::String { displayName.Name });

			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<DisplayName, SerializationException> Deserialize(
			ITokenStreamReader& reader,
			Ludus::Engine::Core::EntityId ownerId
		)
		{
			try
			{
				DisplayName displayName;
				displayName.OwnerId = ownerId;

				Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
				{
					if (key == "Name")
					{
						displayName.Name = std::string(Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data);
						return;
					}

					Ludus::Engine::Serialization::Core::SkipValue(reader);
				});

				return displayName;
			}
			catch (const SerializationException& ex)
			{
				const auto error = Ludus::Engine::Serialization::Core::WithContext(
					ex, "DisplayNameComponentSchema::Deserialize"
				);

				return Ludus::Engine::Core::Unexpected<SerializationException>::Create(error);
			}
		}

	};
}
