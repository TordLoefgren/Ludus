#pragma once

#include <format>
#include <string>

#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Enums.h>
#include <Ludus/Engine/Core/Expected.h>
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

			writer.Emit(Token::Key { "OwnerHandle" });
			writer.Emit(Token::Uint { rigidBody.OwnerHandle });

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

			const std::string type = std::format("{}", rigidBody.Type);
			writer.Emit(Token::Key { "Type" });
			writer.Emit(Token::String { type });

			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<RigidBody, SerializationException> Deserialize(ITokenStreamReader& reader)
		{
			try
			{
				RigidBody rigidBody(0);
				bool hasOwner = false;
				rigidBody.Velocity.X = 0.0f;
				rigidBody.Velocity.Y = 0.0f;
				rigidBody.GravityScale = 1.0f;
				rigidBody.Mass = 1.0f;

				Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
				{
					if (key == "OwnerHandle")
					{
						rigidBody.OwnerHandle = Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader);
						hasOwner = true;
						return;
					}
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
					if (key == "Type")
					{
						std::string typeValue = std::string(
							Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data);
						Ludus::Engine::Physics::Core::BodyType parsed;
						if (Ludus::Engine::Physics::Core::TryParse(typeValue, parsed))
						{
							rigidBody.Type = parsed;
						}
						return;
					}

					Ludus::Engine::Serialization::Core::SkipValue(reader);
				});

				if (!hasOwner)
				{
					throw SerializationException("No owner handle found.");
				}

				return rigidBody;
			}
			catch (const SerializationException& ex)
			{
				const auto error =
					Ludus::Engine::Serialization::Core::WithContext(ex, "RigidBody2DComponentSchema::Deserialize");
				return Ludus::Engine::Core::Expected<RigidBody, SerializationException>(
					Ludus::Engine::Core::Unexpected<SerializationException>::Create(error)
				);
			}
		}
	};
}
