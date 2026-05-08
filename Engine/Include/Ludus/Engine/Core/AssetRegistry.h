#pragma once

#include <span>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <Ludus/Engine/Core/AssetType.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::Engine::Core
{
	class AssetRegistry
	{
	private:
		std::vector<Ludus::Engine::Runtime::AssetReference> m_References;
		std::unordered_map<AssetId, size_t> m_IdToIndex;

	private:
		void CommitAsset(Ludus::Engine::Runtime::AssetReference assetReference)
		{
			if (!assetReference.Id.IsValid())
			{
				throw std::runtime_error("Cannot register an asset with an invalid id.");
			}

			if (BuiltInAssetIds::IsBuiltIn(assetReference.Id))
			{
				throw std::runtime_error("Cannot register a built-in asset id in the project asset registry.");
			}

			if (assetReference.Type == Ludus::Engine::Core::AssetType::Unknown)
			{
				throw std::runtime_error("Cannot register an asset with unknown asset type.");
			}

			if (assetReference.Path.empty())
			{
				throw std::runtime_error("Cannot register an asset with an empty path.");
			}

			if (Contains(assetReference.Id))
			{
				throw std::runtime_error("Cannot register an asset with a duplicate id.");
			}

			const auto assetId = assetReference.Id;

			m_References.push_back(std::move(assetReference));
			m_IdToIndex[assetId] = m_References.size() - 1;
		}

	public:
		explicit AssetRegistry(const Ludus::Engine::Runtime::RuntimeManifest& runtimeManifest)
		{
			m_References.reserve(runtimeManifest.Assets.size());
			m_IdToIndex.reserve(runtimeManifest.Assets.size());

			for (const auto& asset : runtimeManifest.Assets)
			{
				CommitAsset(asset);
			}
		}

		std::span<const Ludus::Engine::Runtime::AssetReference> View() const
		{
			return { m_References.data(), m_References.size() };
		}

		bool Contains(AssetId id) const
		{
			return m_IdToIndex.contains(id);
		}

		const Ludus::Engine::Runtime::AssetReference* TryGetAsset(AssetId id) const
		{
			if (const auto iter = m_IdToIndex.find(id); iter != m_IdToIndex.end())
			{
				return &m_References[iter->second];
			}

			return nullptr;
		}

		const Ludus::Engine::Runtime::AssetReference& GetAsset(AssetId id) const
		{
			const auto* asset = TryGetAsset(id);
			LUDUS_ASSERT(asset != nullptr, "Asset id does not exist.");
			return *asset;
		}
	};
}
