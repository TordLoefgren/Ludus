#pragma once

#include <string>

#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Core/Enums.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Physics/Core/BodyType.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>
#include <Ludus/Engine/Serialization/Core/TokenRead.h>

namespace Ludus::Engine::Serialization::Schemas
{
	using RigidBody = Ludus::Engine::Components::RigidBody2DComponent;
	using ITokenStreamWriter = Ludus::Engine::Serialization::Core::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;
	using SerializationException = Ludus::Engine::Serialization::Core::SerializationException;
	using Token = Ludus::Engine::Serialization::Core::Token;

	struct RigidBody2DComponentSchema
	{
		inline static void Serialize(ITokenStreamWriter& writer, const RigidBody& rigidBody)
		{
			writer.Emit(Token::StartObject { });

			{
				writer.Emit(Token::Key { "Velocity" });
				writer.Emit(Token::StartObject { });
				writer.Emit(Token::Key { "X" });
				writer.Emit(Token::Double { rigidBody.Velocity.X });
				writer.Emit(Token::Key { "Y" });
				writer.Emit(Token::Double { rigidBody.Velocity.Y });
				writer.Emit(Token::EndObject { });
			}

			writer.Emit(Token::Key { "GravityScale" });
			writer.Emit(Token::Double { rigidBody.GravityScale });

			writer.Emit(Token::Key { "Mass" });
			writer.Emit(Token::Double { rigidBody.Mass });

			const std::string bodyType = Ludus::Engine::Core::Enums::GetDisplayName(rigidBody.BodyType);
			writer.Emit(Token::Key { "BodyType" });
			writer.Emit(Token::String { bodyType });

			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<RigidBody, SerializationException> Deserialize(
			ITokenStreamReader& reader,
			Ludus::Engine::Core::EntityId ownerId
		)
		{
			try
			{
				RigidBody rigidBody;
				rigidBody.OwnerId = ownerId;

				Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
				{
					if (key == "Velocity")
					{
						Ludus::Engine::Serialization::Core::ReadObject(reader,
							[&](std::string_view velocityKey)
						{
							if (velocityKey == "X")
							{
								rigidBody.Velocity.X = Ludus::Engine::Serialization::Core::ConsumeFloatLike(reader);
								return;
							}
							if (velocityKey == "Y")
							{
								rigidBody.Velocity.Y = Ludus::Engine::Serialization::Core::ConsumeFloatLike(reader);
								return;
							}

							Ludus::Engine::Serialization::Core::SkipValue(reader);
						});
						return;
					}
					if (key == "GravityScale")
					{
						rigidBody.GravityScale = Ludus::Engine::Serialization::Core::ConsumeFloatLike(reader);
						return;
					}
					if (key == "Mass")
					{
						rigidBody.Mass = Ludus::Engine::Serialization::Core::ConsumeFloatLike(reader);
						return;
					}
					if (key == "BodyType")
					{
						std::string bodyTypeValue = std::string(
							Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data);
						Ludus::Engine::Physics::Core::BodyType parsed;
						if (Ludus::Engine::Physics::Core::TryParse(bodyTypeValue, parsed))
						{
							rigidBody.BodyType = parsed;
						}
						return;
					}

					Ludus::Engine::Serialization::Core::SkipValue(reader);
				});

				return rigidBody;
			}
			catch (const SerializationException& ex)
			{
				const auto error = Ludus::Engine::Serialization::Core::WithContext(
					ex, "RigidBody2DComponentSchema::Deserialize"
				);

				return Ludus::Engine::Core::Unexpected<SerializationException>::Create(error);
			}
		}
	};
}
