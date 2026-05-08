#include "pch.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <unordered_map>

#include <Ludus/Engine/Core/AssetManager.h>
#include <Ludus/Engine/Core/AssetRegistry.h>
#include <Ludus/Engine/Core/AssetType.h>
#include <Ludus/Engine/FileSystem/FileSystem.h>
#include <Ludus/Engine/Graphics/ITextureLoader.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::EngineTests::Core
{
	using AssetId = Ludus::Engine::Core::AssetId;
	using AssetManager = Ludus::Engine::Core::AssetManager;
	using AssetRegistry = Ludus::Engine::Core::AssetRegistry;
	using AssetType = Ludus::Engine::Core::AssetType;
	using DirectoryDeleteScope = Ludus::Engine::FileSystem::DirectoryDeleteScope;
	using ITextureLoader = Ludus::Engine::Graphics::ITextureLoader;
	using RuntimeManifest = Ludus::Engine::Runtime::RuntimeManifest;
	using AssetReference = Ludus::Engine::Runtime::AssetReference;
	using Texture = Ludus::Engine::Graphics::Texture;

	namespace
	{
		class TestTextureLoader final : public ITextureLoader
		{
		private:
			struct TestTextureData
			{
				int Width = 1;
				int Height = 1;
			};

			std::unordered_map<std::filesystem::path, TestTextureData> m_TexturesByPath;
			mutable std::unordered_map<std::filesystem::path, int> m_LoadCountsByPath;

		public:
			TestTextureLoader() = default;

			void SetTexture(std::filesystem::path path, int width, int height)
			{
				m_TexturesByPath.insert_or_assign(
					std::move(path),
					TestTextureData { .Width = width, .Height = height }
				);
			}

			int GetLoadCount(const std::filesystem::path& path) const
			{
				const auto iterator = m_LoadCountsByPath.find(path);
				if (iterator == m_LoadCountsByPath.end())
				{
					return 0;
				}

				return iterator->second;
			}

			std::optional<Texture> TryLoadTextureFromFile(const std::filesystem::path& path) const override
			{
				m_LoadCountsByPath[path]++;

				const auto iterator = m_TexturesByPath.find(path);
				if (iterator == m_TexturesByPath.end())
				{
					return std::nullopt;
				}

				return Texture::Empty(iterator->second.Width, iterator->second.Height);
			}
		};

		struct TestAssetManager
		{
			std::unique_ptr<AssetManager> Manager;
			TestTextureLoader* Loader = nullptr;
		};

		AssetReference MakeTextureAssetReference(AssetId id, std::filesystem::path path)
		{
			return {
				.Id = id,
				.Type = AssetType::Texture2D,
				.Path = std::move(path)
			};
		}

		class AssetManagerFixture : public ::testing::Test
		{
		protected:
			std::filesystem::path m_RuntimeRootDirectory;
			DirectoryDeleteScope m_RuntimeRootDirectoryScope;

			virtual void SetUp() override
			{
				m_RuntimeRootDirectory = std::filesystem::temp_directory_path()
					/ Ludus::Engine::FileSystem::GenerateUniqueName("Ludus-AssetManagerTests-", "");
				std::filesystem::create_directories(m_RuntimeRootDirectory);
				m_RuntimeRootDirectoryScope.Path = m_RuntimeRootDirectory;
			}

			std::filesystem::path CreateRuntimeFile(const std::filesystem::path& relativePath)
			{
				const auto fullPath = m_RuntimeRootDirectory / relativePath;
				std::filesystem::create_directories(fullPath.parent_path());
				Ludus::Engine::FileSystem::WriteAllText(fullPath, "test");
				return fullPath;
			}

			TestAssetManager CreateAssetManager(const AssetRegistry& assetRegistry)
			{
				auto loader = std::make_unique<TestTextureLoader>();
				auto* loaderPtr = loader.get();

				auto manager = std::make_unique<AssetManager>(
					assetRegistry,
					m_RuntimeRootDirectory,
					std::move(loader),
					Texture::Empty(2, 2)
				);

				return {
					.Manager = std::move(manager),
					.Loader = loaderPtr
				};
			}
		};
	}

	TEST_F(AssetManagerFixture, GetTexture2D_When_AssetIdIsInvalid_ReturnsNullWithoutFallback)
	{
		// Arrange.
		const auto runtimeManifest = RuntimeManifest::Create();
		const auto assetRegistry = AssetRegistry(runtimeManifest);
		auto assetManager = CreateAssetManager(assetRegistry);

		// Act.
		const auto result = assetManager.Manager->GetTexture2D(AssetId::Invalid());

		// Assert.
		ASSERT_EQ(result.Texture, nullptr);
		ASSERT_FALSE(result.IsFallback);
	}

	TEST_F(AssetManagerFixture, GetTexture2D_When_RegistryEntryIsMissing_ReturnsFallbackAndCachesFailure)
	{
		// Arrange.
		const auto runtimeManifest = RuntimeManifest::Create();
		const auto assetRegistry = AssetRegistry(runtimeManifest);
		auto assetManager = CreateAssetManager(assetRegistry);
		const auto assetId = AssetId { 101 };

		// Act.
		const auto first = assetManager.Manager->GetTexture2D(assetId);
		const auto second = assetManager.Manager->GetTexture2D(assetId);

		// Assert.
		ASSERT_NE(first.Texture, nullptr);
		ASSERT_TRUE(first.IsFallback);
		ASSERT_TRUE(assetManager.Manager->HasFailedTextureLoad(assetId));
		ASSERT_EQ(first.Texture, second.Texture);
		ASSERT_TRUE(second.IsFallback);
		ASSERT_EQ(first.Texture->Width(), 2);
		ASSERT_EQ(first.Texture->Height(), 2);
		ASSERT_EQ(assetManager.Loader->GetLoadCount(m_RuntimeRootDirectory / "Assets/Missing.png"), 0);
	}

	TEST_F(AssetManagerFixture, GetTexture2D_When_AssetIdIsBuiltInMissingTexture_ReturnsFallbackWithoutCachingFailure)
	{
		// Arrange.
		const auto runtimeManifest = RuntimeManifest::Create();
		const auto assetRegistry = AssetRegistry(runtimeManifest);
		auto assetManager = CreateAssetManager(assetRegistry);

		// Act.
		const auto result = assetManager.Manager->GetTexture2D(Ludus::Engine::Core::BuiltInAssetIds::MissingTexture);

		// Assert.
		ASSERT_NE(result.Texture, nullptr);
		ASSERT_TRUE(result.IsFallback);
		ASSERT_FALSE(assetManager.Manager->HasFailedTextureLoad(Ludus::Engine::Core::BuiltInAssetIds::MissingTexture));
	}

	TEST_F(AssetManagerFixture, GetTexture2D_When_TextureFileIsMissing_ReturnsFallbackAndCachesFailure)
	{
		// Arrange.
		const auto assetId = AssetId { 202 };
		const auto runtimeManifest = RuntimeManifest::Create(
			Ludus::Engine::Core::SceneId::Invalid(),
			std::vector<Ludus::Engine::Runtime::SceneReference> { },
			std::vector<Ludus::Engine::Runtime::ScriptReference> { },
			std::vector<AssetReference> { MakeTextureAssetReference(assetId, "Assets/DoesNotExist.png") }
		);
		const auto assetRegistry = AssetRegistry(runtimeManifest);
		auto assetManager = CreateAssetManager(assetRegistry);

		// Act.
		const auto first = assetManager.Manager->GetTexture2D(assetId);
		const auto second = assetManager.Manager->GetTexture2D(assetId);

		// Assert.
		ASSERT_NE(first.Texture, nullptr);
		ASSERT_TRUE(first.IsFallback);
		ASSERT_TRUE(assetManager.Manager->HasFailedTextureLoad(assetId));
		ASSERT_EQ(first.Texture, second.Texture);
		ASSERT_TRUE(second.IsFallback);
		ASSERT_EQ(assetManager.Loader->GetLoadCount(m_RuntimeRootDirectory / "Assets/DoesNotExist.png"), 0);
	}

	TEST_F(AssetManagerFixture, GetTexture2D_When_TextureLoadsSuccessfully_CachesTexture)
	{
		// Arrange.
		const auto assetId = AssetId { 303 };
		const auto assetPath = std::filesystem::path("Assets/Texture.png");
		const auto fullPath = CreateRuntimeFile(assetPath);
		const auto runtimeManifest = RuntimeManifest::Create(
			Ludus::Engine::Core::SceneId::Invalid(),
			std::vector<Ludus::Engine::Runtime::SceneReference> { },
			std::vector<Ludus::Engine::Runtime::ScriptReference> { },
			std::vector<AssetReference> { MakeTextureAssetReference(assetId, assetPath) }
		);
		const auto assetRegistry = AssetRegistry(runtimeManifest);
		auto assetManager = CreateAssetManager(assetRegistry);
		assetManager.Loader->SetTexture(fullPath, 64, 32);

		// Act.
		const auto first = assetManager.Manager->GetTexture2D(assetId);
		const auto second = assetManager.Manager->GetTexture2D(assetId);

		// Assert.
		ASSERT_NE(first.Texture, nullptr);
		ASSERT_FALSE(first.IsFallback);
		ASSERT_FALSE(assetManager.Manager->HasFailedTextureLoad(assetId));
		ASSERT_EQ(first.Texture, second.Texture);
		ASSERT_FALSE(second.IsFallback);
		ASSERT_EQ(first.Texture->Width(), 64);
		ASSERT_EQ(first.Texture->Height(), 32);
		ASSERT_EQ(assetManager.Loader->GetLoadCount(fullPath), 1);
	}

	TEST(AssetRegistry, Construct_When_ManifestContainsBuiltInAssetId_ThrowsRuntimeError)
	{
		// Arrange.
		const auto runtimeManifest = RuntimeManifest::Create(
			Ludus::Engine::Core::SceneId::Invalid(),
			std::vector<Ludus::Engine::Runtime::SceneReference> { },
			std::vector<Ludus::Engine::Runtime::ScriptReference> { },
			std::vector<AssetReference> {
			MakeTextureAssetReference(Ludus::Engine::Core::BuiltInAssetIds::MissingTexture, "Assets/Missing.png")
		}
		);

		// Act & Assert.
		ASSERT_THROW((void)AssetRegistry { runtimeManifest }, std::runtime_error);
	}
}
