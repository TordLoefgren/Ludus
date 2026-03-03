#pragma once

#include <Ludus/Engine/Components/Collider2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Physics/Core/LayerMask.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>
#include <Ludus/Engine/Serialization/Core/TokenRead.h>

namespace Ludus::Engine::Serialization::Schemas
{
	using Collider = Ludus::Engine::Components::Collider2DComponent;
	using ITokenStreamWriter = Ludus::Engine::Serialization::Core::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;
	using SerializationException = Ludus::Engine::Serialization::Core::SerializationException;
	using Token = Ludus::Engine::Serialization::Core::Token;

	struct Collider2DComponentSchema
	{
		inline static void Serialize(ITokenStreamWriter& writer, const Collider& collider)
		{
			writer.Emit(Token::StartObject { });

			writer.Emit(Token::Key { "OwnerHandle" });
			writer.Emit(Token::Uint { collider.OwnerHandle });

			writer.Emit(Token::Key { "LayerIndex" });
			writer.Emit(Token::Uint { static_cast<uint32_t>(collider.LayerIndex) });

			writer.Emit(Token::Key { "CollidesWith" });
			writer.Emit(Token::Uint { collider.CollidesWith.Value });

			writer.Emit(Token::Key { "IsTrigger" });
			writer.Emit(Token::Bool { collider.IsTrigger });

			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<Collider, SerializationException> Deserialize(ITokenStreamReader& reader)
		{
			try
			{
				Collider collider;
				bool hasOwner = false;

				Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
				{
					if (key == "OwnerHandle")
					{
						collider.OwnerHandle = Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader);
						hasOwner = true;
						return;
					}
					if (key == "LayerIndex")
					{
						collider.LayerIndex = static_cast<uint8_t>(Ludus::Engine::Serialization::Core::ConsumeUint32Like(reader));
						return;
					}
					if (key == "CollidesWith")
					{
						const auto mask = Ludus::Engine::Serialization::Core::ConsumeUint32Like(reader);
						collider.CollidesWith = Ludus::Engine::Physics::Core::LayerMask(mask);
						return;
					}
					if (key == "IsTrigger")
					{
						collider.IsTrigger = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Bool>(reader).Data;
						return;
					}

					Ludus::Engine::Serialization::Core::SkipValue(reader);
				}
				);

				if (!hasOwner)
				{
					throw SerializationException("No owner handle found.");
				}

				return collider;
			}
			catch (const SerializationException& ex)
			{
				const auto error = Ludus::Engine::Serialization::Core::WithContext(
					ex, "Collider2DComponentSchema::Deserialize"
				);

				return Ludus::Engine::Core::Unexpected<SerializationException>::Create(error);
			}
		}
	};
}
