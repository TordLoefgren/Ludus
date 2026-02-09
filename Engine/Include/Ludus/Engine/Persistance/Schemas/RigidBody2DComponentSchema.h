#pragma once

#include <format>
#include <string>

#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Enums.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Persistance/PersistanceException.h>
#include <Ludus/Engine/Persistance/Serialization/ITokenStreamReader.h>
#include <Ludus/Engine/Persistance/Serialization/ITokenStreamWriter.h>
#include <Ludus/Engine/Persistance/Serialization/TokenRead.h>
#include <Ludus/Engine/Physics/Core/BodyType.h>

namespace Ludus::Engine::Persistance::Schemas
{
	using RigidBody = Ludus::Engine::Components::RigidBody2DComponent;
	using ITokenStreamWriter = Ludus::Engine::Persistance::Serialization::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Persistance::Serialization::ITokenStreamReader;
	using Token = Ludus::Engine::Persistance::Serialization::Token;

	struct RigidBody2DComponentSchema
	{
		inline static void Serialize(ITokenStreamWriter& writer, const RigidBody& rigidBody)
		{
			writer.Emit(Token::StartObject { });

			writer.Emit(Token::Key { "OwnerHandle" });
			writer.Emit(Token::Uint32 { rigidBody.OwnerHandle });

			{
				writer.Emit(Token::Key { "Velocity" });
				writer.Emit(Token::StartObject { });
				writer.Emit(Token::Key { "X" });
				writer.Emit(Token::Float { rigidBody.Velocity.X });
				writer.Emit(Token::Key { "Y" });
				writer.Emit(Token::Float { rigidBody.Velocity.Y });
				writer.Emit(Token::EndObject { });
			}

			writer.Emit(Token::Key { "GravityScale" });
			writer.Emit(Token::Float { rigidBody.GravityScale });

			writer.Emit(Token::Key { "Mass" });
			writer.Emit(Token::Float { rigidBody.Mass });

			const std::string type = std::format("{}", rigidBody.Type);
			writer.Emit(Token::Key { "Type" });
			writer.Emit(Token::String { type });

			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<RigidBody, PersistanceException> Deserialize(ITokenStreamReader& reader)
		{
			try
			{
				RigidBody rigidBody(0);
				bool hasOwner = false;
				rigidBody.Velocity.X = 0.0f;
				rigidBody.Velocity.Y = 0.0f;
				rigidBody.GravityScale = 1.0f;
				rigidBody.Mass = 1.0f;

				Ludus::Engine::Persistance::Serialization::ReadObject(reader,
					[&](std::string_view key)
					{
						if (key == "OwnerHandle")
						{
							rigidBody.OwnerHandle = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Uint32>(reader).Data;
							hasOwner = true;
							return;
						}
						if (key == "Velocity")
						{
							Ludus::Engine::Persistance::Serialization::ReadObject(reader,
								[&](std::string_view velocityKey)
								{
									if (velocityKey == "X")
									{
										rigidBody.Velocity.X = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Float>(reader).Data;
										return;
									}
									if (velocityKey == "Y")
									{
										rigidBody.Velocity.Y = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Float>(reader).Data;
										return;
									}

									Ludus::Engine::Persistance::Serialization::SkipValue(reader);
								});
							return;
						}
						if (key == "GravityScale")
						{
							rigidBody.GravityScale = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Float>(reader).Data;
							return;
						}
						if (key == "Mass")
						{
							rigidBody.Mass = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Float>(reader).Data;
							return;
						}
						if (key == "Type")
						{
							std::string typeValue = std::string(
								Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::String>(reader).Data);
							Ludus::Engine::Physics::Core::BodyType parsed;
							if (Ludus::Engine::Physics::Core::TryParse(typeValue, parsed))
							{
								rigidBody.Type = parsed;
							}
							return;
						}

						Ludus::Engine::Persistance::Serialization::SkipValue(reader);
					});

				if (!hasOwner)
				{
					throw PersistanceException("No owner handle found.");
				}

				return rigidBody;
			}
			catch (const PersistanceException& ex)
			{
				return Ludus::Engine::Core::Expected<RigidBody, PersistanceException>(
					Ludus::Engine::Core::Unexpected<PersistanceException>::Create(ex)
				);
			}
		}
	};
}
