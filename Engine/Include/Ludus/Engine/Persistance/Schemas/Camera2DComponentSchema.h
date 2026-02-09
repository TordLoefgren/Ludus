#pragma once

#include <string>
#include <variant>

#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Persistance/PersistanceException.h>
#include <Ludus/Engine/Persistance/Serialization/ITokenStreamReader.h>
#include <Ludus/Engine/Persistance/Serialization/ITokenStreamWriter.h>
#include <Ludus/Engine/Persistance/Serialization/TokenRead.h>

namespace Ludus::Engine::Persistance::Schemas
{
	using Camera = Ludus::Engine::Components::Camera2DComponent;
	using ITokenStreamWriter = Ludus::Engine::Persistance::Serialization::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Persistance::Serialization::ITokenStreamReader;
	using Token = Ludus::Engine::Persistance::Serialization::Token;

	struct Camera2DComponentSchema
	{
		inline static void Serialize(ITokenStreamWriter& writer, const Camera& camera)
		{
			writer.Emit(Token::StartObject { });

			writer.Emit(Token::Key { "OwnerHandle" });
			writer.Emit(Token::Uint32 { camera.OwnerHandle });

			writer.Emit(Token::Key { "OrthographicSize" });
			writer.Emit(Token::Float { camera.OrthographicSize });

			writer.Emit(Token::Key { "Priority" });
			writer.Emit(Token::Int { camera.Priority });

			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<Camera, PersistanceException> Deserialize(ITokenStreamReader& reader)
		{
			try
			{
				Camera camera(0);

				bool hasOwner = false;

				Ludus::Engine::Persistance::Serialization::ReadObject(reader,
					[&](std::string_view key)
					{
						if (key == "OwnerHandle")
						{
							camera.OwnerHandle = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Uint32>(reader).Data;
							hasOwner = true;
							return;
						}
						if (key == "OrthographicSize")
						{
							camera.OrthographicSize = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Float>(reader).Data;
							return;
						}
						if (key == "Priority")
						{
							camera.Priority = Ludus::Engine::Persistance::Serialization::ConsumeAs<Token::Int>(reader).Data;
							return;
						}

						Ludus::Engine::Persistance::Serialization::SkipValue(reader);
					});

				if (!hasOwner)
				{
					throw PersistanceException("No owner handle found.");
				}

				return camera;
			}
			catch (const PersistanceException& ex)
			{
				return Ludus::Engine::Core::Expected<Camera, PersistanceException>(
					Ludus::Engine::Core::Unexpected<PersistanceException>::Create(ex)
				);
			}
		}

	};
}
