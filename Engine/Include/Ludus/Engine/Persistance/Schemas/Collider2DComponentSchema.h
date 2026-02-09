#pragma once

#include <Ludus/Engine/Components/Collider2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Persistance/PersistanceException.h>
#include <Ludus/Engine/Persistance/Serialization/ITokenStreamReader.h>
#include <Ludus/Engine/Persistance/Serialization/ITokenStreamWriter.h>
#include <Ludus/Engine/Persistance/Serialization/TokenRead.h>
#include <Ludus/Engine/Physics/Core/LayerMask.h>

namespace Ludus::Engine::Persistance::Schemas
{
	using Collider = Ludus::Engine::Components::Collider2DComponent;
	using ITokenStreamWriter = Ludus::Engine::Persistance::Serialization::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Persistance::Serialization::ITokenStreamReader;
	using Token = Ludus::Engine::Persistance::Serialization::Token;

	struct Collider2DComponentSchema
	{
		inline static void Serialize(ITokenStreamWriter& writer, const Collider& collider)
		{
			writer.Emit(Token::StartObject { });

			writer.Emit(Token::Key { "OwnerHandle" });
			writer.Emit(Token::Uint32 { collider.OwnerHandle });

			writer.Emit(Token::Key { "LayerIndex" });
			writer.Emit(Token::Uint32 { static_cast<uint32_t>(collider.LayerIndex) });

			writer.Emit(Token::Key { "CollidesWith" });
			writer.Emit(Token::Uint32 { collider.CollidesWith.Value });

			writer.Emit(Token::Key { "IsTrigger" });
			writer.Emit(Token::Bool { collider.IsTrigger });

			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<Collider, PersistanceException> Deserialize(ITokenStreamReader& reader)
		{
			try
			{
				Collider collider(0);
				bool hasOwner = false;

				Ludus::Engine::Persistance::Serialization::ReadObject(reader,
					[&](std::string_view key)
					{
						if (key == "OwnerHandle")
						{
							collider.OwnerHandle = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Uint32>(reader).Data;
							hasOwner = true;
							return;
						}
						if (key == "LayerIndex")
						{
							collider.LayerIndex = static_cast<uint8_t>(Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Uint32>(reader).Data);
							return;
						}
						if (key == "CollidesWith")
						{
							const auto mask = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Uint32>(reader).Data;
							collider.CollidesWith = Ludus::Engine::Physics::Core::LayerMask(mask);
							return;
						}
						if (key == "IsTrigger")
						{
							collider.IsTrigger = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Bool>(reader).Data;
							return;
						}

						Ludus::Engine::Persistance::Serialization::SkipValue(reader);
					});

				if (!hasOwner)
				{
					throw PersistanceException("No owner handle found.");
				}

				return collider;
			}
			catch (const PersistanceException& ex)
			{
				return Ludus::Engine::Core::Expected<Collider, PersistanceException>(
					Ludus::Engine::Core::Unexpected<PersistanceException>::Create(ex)
				);
			}
		}
	};
}
