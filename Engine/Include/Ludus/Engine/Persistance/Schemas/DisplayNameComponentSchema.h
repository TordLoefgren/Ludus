#pragma once

#include <Ludus/Engine/Components/DisplayNameComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Persistance/PersistanceException.h>
#include <Ludus/Engine/Persistance/Serialization/ITokenStreamReader.h>
#include <Ludus/Engine/Persistance/Serialization/ITokenStreamWriter.h>
#include <Ludus/Engine/Persistance/Serialization/TokenRead.h>

namespace Ludus::Engine::Persistance::Schemas
{
	using DisplayName = Ludus::Engine::Components::DisplayNameComponent;
	using ITokenStreamWriter = Ludus::Engine::Persistance::Serialization::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Persistance::Serialization::ITokenStreamReader;
	using Token = Ludus::Engine::Persistance::Serialization::Token;

	struct DisplayNameComponentSchema
	{
		inline static void Serialize(ITokenStreamWriter& writer, const DisplayName& displayName)
		{
			writer.Emit(Token::StartObject { });

			writer.Emit(Token::Key { "OwnerHandle" });
			writer.Emit(Token::Uint32 { displayName.OwnerHandle });

			writer.Emit(Token::Key { "Value" });
			writer.Emit(Token::String { displayName.Value });

			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<DisplayName, PersistanceException> Deserialize(ITokenStreamReader& reader)
		{
			try
			{
				DisplayName displayName(0);
				bool hasOwner = false;

				Ludus::Engine::Persistance::Serialization::ReadObject(reader,
					[&](std::string_view key)
					{
						if (key == "OwnerHandle")
						{
							displayName.OwnerHandle = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Uint32>(reader).Data;
							hasOwner = true;
							return;
						}
						if (key == "Value")
						{
							displayName.Value = std::string(Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::String>(reader).Data);
							return;
						}

						Ludus::Engine::Persistance::Serialization::SkipValue(reader);
					});

				if (!hasOwner)
				{
					throw PersistanceException("No owner handle found.");
				}

				return displayName;
			}
			catch (const PersistanceException& ex)
			{
				return Ludus::Engine::Core::Expected<DisplayName, PersistanceException>(
					Ludus::Engine::Core::Unexpected<PersistanceException>::Create(ex)
				);
			}
		}

	};
}
