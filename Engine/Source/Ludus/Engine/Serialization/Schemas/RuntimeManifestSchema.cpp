#include "pch.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <utility>

#include <Ludus/Engine/Core/AssetType.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>
#include <Ludus/Engine/Serialization/Core/TokenRead.h>
#include <Ludus/Engine/Serialization/Schemas/RuntimeManifestSchema.h>

namespace Ludus::Engine::Serialization::Schemas
{
	using Token = Ludus::Engine::Serialization::Core::Token;
	using SceneReference = Ludus::Engine::Runtime::SceneReference;
	using ScriptReference = Ludus::Engine::Runtime::ScriptReference;
	using AssetReference = Ludus::Engine::Runtime::AssetReference;

	void RuntimeManifestSchema::Serialize(ITokenStreamWriter& writer, const RuntimeManifest& runtimeManifest)
	{
		writer.Emit(Token::StartObject { });

		writer.Emit(Token::Key { "SchemaRevision" });
		writer.Emit(Token::Uint { runtimeManifest.SchemaRevision });

		writer.Emit(Token::Key { "EntrySceneId" });
		writer.Emit(Token::Uint { runtimeManifest.EntrySceneId.Value });

		writer.Emit(Token::Key { "Scenes" });
		writer.Emit(Token::StartArray { });

		for (const auto& scene : runtimeManifest.Scenes)
		{
			writer.Emit(Token::StartObject { });

			writer.Emit(Token::Key { "Id" });
			writer.Emit(Token::Uint { scene.Id.Value });

			writer.Emit(Token::Key { "Name" });
			writer.Emit(Token::String { scene.Name });

			writer.Emit(Token::Key { "Path" });
			const auto scenePath = Ludus::Engine::FileSystem::ToPortablePathString(scene.Path);
			writer.Emit(Token::String { scenePath });

			writer.Emit(Token::EndObject { });
		}

		writer.Emit(Token::EndArray { });

		writer.Emit(Token::Key { "Scripts" });
		writer.Emit(Token::StartArray { });

		for (const auto& script : runtimeManifest.Scripts)
		{
			writer.Emit(Token::StartObject { });

			writer.Emit(Token::Key { "Id" });
			writer.Emit(Token::Uint { script.Id.Value });

			writer.Emit(Token::Key { "Name" });
			writer.Emit(Token::String { script.Name });

			writer.Emit(Token::EndObject { });
		}

		writer.Emit(Token::EndArray { });

		writer.Emit(Token::Key { "Assets" });
		writer.Emit(Token::StartArray { });

		for (const auto& asset : runtimeManifest.Assets)
		{
			writer.Emit(Token::StartObject { });

			writer.Emit(Token::Key { "Id" });
			writer.Emit(Token::Uint { asset.Id.Value });

			const std::string assetType = Ludus::Engine::Core::Enums::GetDisplayName(asset.Type);
			writer.Emit(Token::Key { "Type" });
			writer.Emit(Token::String { assetType });

			writer.Emit(Token::Key { "Path" });
			const auto assetPath = Ludus::Engine::FileSystem::ToPortablePathString(asset.Path);
			writer.Emit(Token::String { assetPath });

			writer.Emit(Token::EndObject { });
		}

		writer.Emit(Token::EndArray { });

		writer.Emit(Token::EndObject { });
	}

	Ludus::Engine::Core::Expected<RuntimeManifest, SerializationException> RuntimeManifestSchema::Deserialize(ITokenStreamReader& reader)
	{
		RuntimeManifest runtimeManifest;

		try
		{
			bool hasSchemaRevision = false;
			bool hasEntrySceneId = false;

			Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
			{
				if (key == "SchemaRevision")
				{
					runtimeManifest.SchemaRevision = Ludus::Engine::Serialization::Core::ConsumeUint32Like(reader);
					if (runtimeManifest.SchemaRevision != RuntimeManifest::CurrentSchemaRevision)
					{
						throw SerializationException("RuntimeManifest schema revision is not supported.");
					}

					hasSchemaRevision = true;
					return;
				}
				if (key == "Scenes")
				{
					Ludus::Engine::Serialization::Core::ConsumeAs<Token::StartArray>(reader);

					while (!Ludus::Engine::Serialization::Core::Is<Token::EndArray>(reader.Peek()))
					{
						SceneReference scene;
						bool hasId = false;
						bool hasName = false;
						bool hasPath = false;

						Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view sceneKey)
						{
							if (sceneKey == "Id")
							{
								scene.Id = { Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader) };
								if (!scene.Id.IsValid())
								{
									throw SerializationException("RuntimeManifest scene id is invalid.");
								}
								hasId = true;
								return;
							}
							if (sceneKey == "Name")
							{
								scene.Name = std::string(Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data);
								hasName = true;
								return;
							}
							if (sceneKey == "Path")
							{
								scene.Path = std::filesystem::path(
									std::string(Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data)
								);
								hasPath = true;
								return;
							}

							Ludus::Engine::Serialization::Core::SkipValue(reader);
						});

						if (!hasId || !hasName || !hasPath)
						{
							throw SerializationException("RuntimeManifest scene entry is incomplete.");
						}

						runtimeManifest.Scenes.emplace_back(std::move(scene));
					}

					Ludus::Engine::Serialization::Core::ConsumeAs<Token::EndArray>(reader);
					return;
				}
				if (key == "EntrySceneId")
				{
					runtimeManifest.EntrySceneId = { Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader) };
					hasEntrySceneId = true;
					return;
				}
				if (key == "Scripts")
				{
					Ludus::Engine::Serialization::Core::ConsumeAs<Token::StartArray>(reader);

					while (!Ludus::Engine::Serialization::Core::Is<Token::EndArray>(reader.Peek()))
					{
						ScriptReference script;
						bool hasId = false;
						bool hasName = false;

						Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view scriptKey)
						{
							if (scriptKey == "Id")
							{
								script.Id = { Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader) };
								if (!script.Id.IsValid())
								{
									throw SerializationException("RuntimeManifest script id is invalid.");
								}
								hasId = true;
								return;
							}
							if (scriptKey == "Name")
							{
								script.Name = std::string(Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data);
								hasName = true;
								return;
							}

							Ludus::Engine::Serialization::Core::SkipValue(reader);
						});

						if (!hasId || !hasName)
						{
							throw SerializationException("RuntimeManifest script entry is incomplete.");
						}

						runtimeManifest.Scripts.emplace_back(std::move(script));
					}

					Ludus::Engine::Serialization::Core::ConsumeAs<Token::EndArray>(reader);
					return;
				}
				if (key == "Assets")
				{
					Ludus::Engine::Serialization::Core::ConsumeAs<Token::StartArray>(reader);

					while (!Ludus::Engine::Serialization::Core::Is<Token::EndArray>(reader.Peek()))
					{
						AssetReference asset;
						bool hasId = false;
						bool hasType = false;
						bool hasPath = false;

						Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view assetKey)
						{
							if (assetKey == "Id")
							{
								asset.Id = { Ludus::Engine::Serialization::Core::ConsumeUint64Like(reader) };
								if (!asset.Id.IsValid())
								{
									throw SerializationException("RuntimeManifest asset id is invalid.");
								}
								if (Ludus::Engine::Core::BuiltInAssetIds::IsBuiltIn(asset.Id))
								{
									throw SerializationException("RuntimeManifest asset id cannot use the built-in asset range.");
								}
								hasId = true;
								return;
							}
							if (assetKey == "Type")
							{
								std::string assetTypeValue = std::string(
									Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data);
								Ludus::Engine::Core::AssetType parsed;
								if (!Ludus::Engine::Core::TryParse(assetTypeValue, parsed))
								{
									throw SerializationException("RuntimeManifest asset type is invalid.");
								}

								if (parsed == Ludus::Engine::Core::AssetType::Unknown)
								{
									throw SerializationException("RuntimeManifest asset type cannot be Unknown.");
								}

								asset.Type = parsed;
								hasType = true;
								return;
							}
							if (assetKey == "Path")
							{
								asset.Path = std::filesystem::path(
									std::string(Ludus::Engine::Serialization::Core::ConsumeAs<Token::String>(reader).Data)
								);
								hasPath = true;
								return;
							}

							Ludus::Engine::Serialization::Core::SkipValue(reader);
						});

						if (!hasId || !hasType || !hasPath)
						{
							throw SerializationException("RuntimeManifest asset entry is incomplete.");
						}

						runtimeManifest.Assets.emplace_back(std::move(asset));
					}

					Ludus::Engine::Serialization::Core::ConsumeAs<Token::EndArray>(reader);
					return;
				}

				Ludus::Engine::Serialization::Core::SkipValue(reader);
			});

			if (!hasSchemaRevision)
			{
				throw SerializationException("RuntimeManifest schema revision not found.");
			}
			if (!hasEntrySceneId)
			{
				throw SerializationException("RuntimeManifest entry scene id not found.");
			}
			if (!runtimeManifest.EntrySceneId.IsValid())
			{
				return runtimeManifest;
			}

			for (const auto& scene : runtimeManifest.Scenes)
			{
				if (scene.Id == runtimeManifest.EntrySceneId)
				{
					return runtimeManifest;
				}
			}

			throw SerializationException("RuntimeManifest entry scene id was not found in scene references.");
		}
		catch (const SerializationException& ex)
		{
			const auto error = Ludus::Engine::Serialization::Core::WithContext(
				ex, "RuntimeManifestSchema::Deserialize"
			);

			return Ludus::Engine::Core::Unexpected<SerializationException>::Create(error);
		}
	}
}
