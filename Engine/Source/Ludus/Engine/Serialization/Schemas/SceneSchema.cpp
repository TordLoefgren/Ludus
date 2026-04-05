#include "pch.h"

#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Components/Collider2DComponent.h>
#include <Ludus/Engine/Components/DisplayNameComponent.h>
#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Components/Text2DComponent.h>
#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/Random.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Serialization/Core/TokenRead.h>
#include <Ludus/Engine/Serialization/Schemas/Camera2DComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/Collider2DComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/DisplayNameComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/RigidBody2DComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/SceneSchema.h>
#include <Ludus/Engine/Serialization/Schemas/ScriptComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/Sprite2DComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/Text2DComponentSchema.h>
#include <Ludus/Engine/Serialization/Schemas/Transform2DComponentSchema.h>

namespace
{
	using Token = Ludus::Engine::Serialization::Core::Token;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;

	struct StagedComponents
	{
		std::optional<Ludus::Engine::Components::Camera2DComponent> Camera;
		std::optional<Ludus::Engine::Components::Collider2DComponent> Collider;
		std::optional<Ludus::Engine::Components::DisplayNameComponent> DisplayName;
		std::optional<Ludus::Engine::Components::RigidBody2DComponent> RigidBody;
		std::optional<Ludus::Engine::Components::ScriptComponent> Script;
		std::optional<Ludus::Engine::Components::Sprite2DComponent> Sprite;
		std::optional<Ludus::Engine::Components::Text2DComponent> Text;
		std::optional<Ludus::Engine::Components::Transform2DComponent> Transform;
	};

	template <class SlotT, class LoaderT>
	void TryReadComponent(
		ITokenStreamReader& reader,
		SlotT& componentSlot,
		LoaderT loader,
		std::string_view componentName
	)
	{
		if (!Ludus::Engine::Serialization::Core::Is<Token::StartObject>(reader.Peek()))
		{
			Ludus::Engine::Serialization::Core::SkipValue(reader);
			return;
		}

		const auto result = loader(reader);
		if (!result.HasValue())
		{
			LUDUS_LOG_WARN(
				std::string("SceneSchema: failed to read component ") +
				std::string(componentName) + ": " + result.GetError().what()
			);

			if (!Ludus::Engine::Serialization::Core::Is<Token::Key>(reader.Peek()) &&
				!Ludus::Engine::Serialization::Core::Is<Token::EndObject>(reader.Peek()) &&
				!Ludus::Engine::Serialization::Core::Is<Token::EndArray>(reader.Peek()))
			{
				Ludus::Engine::Serialization::Core::SkipValue(reader);
			}
			return;
		}

		componentSlot = std::move(result.GetValue());
	}
}

namespace Ludus::Engine::Serialization::Schemas
{
	void SceneSchema::Serialize(ITokenStreamWriter& writer, const Scene& scene)
	{
		writer.Emit(Token::StartObject { });

		writer.Emit(Token::Key { "Id" });
		writer.Emit(Token::Uint { scene.Id.Value });

		writer.Emit(Token::Key { "Name" });
		writer.Emit(Token::String { scene.Name });

		writer.Emit(Token::Key { "Entities" });
		writer.Emit(Token::StartArray { });

		auto& ecs = scene.EntityComponentSystem;
		for (const auto& entity : ecs.View())
		{
			writer.Emit(Token::StartObject { });

			writer.Emit(Token::Key { "Id" });
			writer.Emit(Token::Uint { entity.Id.Value });

			if (const auto* camera = ecs.Cameras.TryGetByOwner(entity.Id))
			{
				writer.Emit(Token::Key { Camera2DString });
				Camera2DComponentSchema::Serialize(writer, *camera);
			}

			if (const auto* collider = ecs.Colliders.TryGetByOwner(entity.Id))
			{
				writer.Emit(Token::Key { Collider2DString });
				Collider2DComponentSchema::Serialize(writer, *collider);
			}

			if (const auto* displayName = ecs.DisplayNames.TryGetByOwner(entity.Id))
			{
				writer.Emit(Token::Key { DisplayNameString });
				DisplayNameComponentSchema::Serialize(writer, *displayName);
			}

			if (const auto* rigidBody = ecs.RigidBodies.TryGetByOwner(entity.Id))
			{
				writer.Emit(Token::Key { RigidBody2DString });
				RigidBody2DComponentSchema::Serialize(writer, *rigidBody);
			}

			if (const auto* script = ecs.Scripts.TryGetByOwner(entity.Id))
			{
				writer.Emit(Token::Key { ScriptString });
				ScriptComponentSchema::Serialize(writer, *script);
			}

			if (const auto* sprite = ecs.Sprites.TryGetByOwner(entity.Id))
			{
				writer.Emit(Token::Key { Sprite2DString });
				Sprite2DComponentSchema::Serialize(writer, *sprite);
			}

			if (const auto* text = ecs.Texts.TryGetByOwner(entity.Id))
			{
				writer.Emit(Token::Key { Text2DString });
				Text2DComponentSchema::Serialize(writer, *text);
			}

			if (const auto* transform = ecs.Transforms.TryGetByOwner(entity.Id))
			{
				writer.Emit(Token::Key { Transform2DString });
				Transform2DComponentSchema::Serialize(writer, *transform);
			}

			writer.Emit(Token::EndObject { });
		}

		writer.Emit(Token::EndArray { });
		writer.Emit(Token::EndObject { });
	}

	Ludus::Engine::Core::Expected<Scene, SerializationException> SceneSchema::Deserialize(ITokenStreamReader& reader)
	{
		Scene scene { };
		try
		{
			bool hasId = false;

			Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
			{
				if (key == "Id")
				{
					scene.Id = { Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader) };
					if (!scene.Id.IsValid())
					{
						throw SerializationException("Scene id is invalid.");
					}

					hasId = true;
					return;
				}
				if (key == "Name")
				{
					scene.Name = Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data;
					return;
				}
				if (key == "Entities")
				{
					Ludus::Engine::Serialization::Core::ConsumeAs<Token::StartArray>(reader);

					while (!Ludus::Engine::Serialization::Core::Is<Token::EndArray>(reader.Peek()))
					{
						StagedComponents stagedComponents;
						Ludus::Engine::Core::EntityId entityId { Ludus::Engine::Core::EntityId::Invalid() };
						bool hasEntityId = false;

						Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view entityKey)
						{
							if (entityKey == "Id")
							{
								entityId = { Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader) };
								if (!entityId.IsValid())
								{
									throw SerializationException("Entity id is invalid.");
								}
								hasEntityId = true;
								return;
							}
							if (entityKey == Camera2DString)
							{
								TryReadComponent(
									reader,
									stagedComponents.Camera,
									[](auto& componentReader) { return Camera2DComponentSchema::Deserialize(componentReader, Ludus::Engine::Core::EntityId::Invalid()); },
									"Camera2D"
								);
								return;
							}
							if (entityKey == Collider2DString)
							{
								TryReadComponent(
									reader,
									stagedComponents.Collider,
									[](auto& componentReader) { return Collider2DComponentSchema::Deserialize(componentReader, Ludus::Engine::Core::EntityId::Invalid()); },
									"Collider2D"
								);
								return;
							}
							if (entityKey == DisplayNameString)
							{
								TryReadComponent(
									reader,
									stagedComponents.DisplayName,
									[](auto& componentReader) { return DisplayNameComponentSchema::Deserialize(componentReader, Ludus::Engine::Core::EntityId::Invalid()); },
									"DisplayName"
								);
								return;
							}
							if (entityKey == RigidBody2DString)
							{
								TryReadComponent(
									reader,
									stagedComponents.RigidBody,
									[](auto& componentReader) { return RigidBody2DComponentSchema::Deserialize(componentReader, Ludus::Engine::Core::EntityId::Invalid()); },
									"RigidBody2D"
								);
								return;
							}
							if (entityKey == ScriptString)
							{
								TryReadComponent(
									reader,
									stagedComponents.Script,
									[](auto& componentReader) { return ScriptComponentSchema::Deserialize(componentReader, Ludus::Engine::Core::EntityId::Invalid()); },
									"Script"
								);
								return;
							}
							if (entityKey == Sprite2DString)
							{
								TryReadComponent(
									reader,
									stagedComponents.Sprite,
									[](auto& componentReader) { return Sprite2DComponentSchema::Deserialize(componentReader, Ludus::Engine::Core::EntityId::Invalid()); },
									"Sprite2D"
								);
								return;
							}
							if (entityKey == Text2DString)
							{
								TryReadComponent(
									reader,
									stagedComponents.Text,
									[](auto& componentReader) { return Text2DComponentSchema::Deserialize(componentReader, Ludus::Engine::Core::EntityId::Invalid()); },
									"Text2D"
								);
								return;
							}
							if (entityKey == Transform2DString)
							{
								TryReadComponent(
									reader,
									stagedComponents.Transform,
									[](auto& componentReader) { return Transform2DComponentSchema::Deserialize(componentReader, Ludus::Engine::Core::EntityId::Invalid()); },
									"Transform2D"
								);
								return;
							}

							Ludus::Engine::Serialization::Core::SkipValue(reader);
						});

						if (!hasEntityId)
						{
							continue;
						}

						scene.EntityComponentSystem.RestoreEntity(entityId);

						if (stagedComponents.Camera)
						{
							stagedComponents.Camera->OwnerId = entityId;
							scene.EntityComponentSystem.AttachCamera(*stagedComponents.Camera);
						}

						if (stagedComponents.Collider)
						{
							stagedComponents.Collider->OwnerId = entityId;
							scene.EntityComponentSystem.AttachCollider(*stagedComponents.Collider);
						}

						if (stagedComponents.DisplayName)
						{
							stagedComponents.DisplayName->OwnerId = entityId;
							scene.EntityComponentSystem.AttachDisplayName(*stagedComponents.DisplayName);
						}

						if (stagedComponents.RigidBody)
						{
							stagedComponents.RigidBody->OwnerId = entityId;
							scene.EntityComponentSystem.AttachRigidBody(*stagedComponents.RigidBody);
						}

						if (stagedComponents.Script)
						{
							stagedComponents.Script->OwnerId = entityId;
							scene.EntityComponentSystem.AttachScript(*stagedComponents.Script);
						}

						if (stagedComponents.Sprite)
						{
							stagedComponents.Sprite->OwnerId = entityId;
							scene.EntityComponentSystem.AttachSprite(*stagedComponents.Sprite);
						}

						if (stagedComponents.Text)
						{
							stagedComponents.Text->OwnerId = entityId;
							scene.EntityComponentSystem.AttachText(*stagedComponents.Text);
						}

						if (stagedComponents.Transform)
						{
							stagedComponents.Transform->OwnerId = entityId;
							scene.EntityComponentSystem.AttachTransform(*stagedComponents.Transform);
						}
					}

					Ludus::Engine::Serialization::Core::ConsumeAs<Token::EndArray>(reader);
					return;
				}

				Ludus::Engine::Serialization::Core::SkipValue(reader);
			});

			if (!hasId)
			{
				throw SerializationException("No scene id found.");
			}
		}
		catch (const SerializationException& ex)
		{
			const auto error = Ludus::Engine::Serialization::Core::WithContext(
				ex, "SceneSchema::Deserialize"
			);

			return Ludus::Engine::Core::Unexpected<SerializationException>::Create(error);
		}

		return scene;
	}
}
