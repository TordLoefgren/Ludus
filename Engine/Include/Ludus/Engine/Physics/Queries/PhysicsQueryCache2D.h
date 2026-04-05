#pragma once

#include <span>
#include <unordered_map>
#include <vector>

#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Physics/Narrowphase/ContactPair2D.h>
#include <Ludus/Engine/Physics/Queries/IPhysicsQueryCache2D.h>

namespace Ludus::Engine::Physics::Queries
{
	using EntityId = Ludus::Engine::Core::EntityId;
	using ContactPair2D = Ludus::Engine::Physics::Narrowphase::ContactPair2D;

	class PhysicsQueryCache2D : public Ludus::Engine::Physics::Queries::IPhysicsQueryCache2D
	{
	private:
		std::unordered_map<EntityId, std::vector<ContactPair2D>> m_ContactsByEntity;

		static bool PairInvolves(const ContactPair2D& pair, EntityId a, EntityId b)
		{
			return (pair.EntityIdA == a && pair.EntityIdB == b) ||
				(pair.EntityIdA == b && pair.EntityIdB == a);
		}

	public:
		virtual void UpdateFromContacts(const std::vector <ContactPair2D> contactPairs)
		{
			m_ContactsByEntity.reserve(
				m_ContactsByEntity.size() + contactPairs.size() * 2
			);

			for (const auto& contact : contactPairs)
			{
				const EntityId idA = contact.EntityIdA;
				const EntityId idB = contact.EntityIdB;

				m_ContactsByEntity[idA].push_back(contact);
				m_ContactsByEntity[idB].push_back(contact);
			}
		}

		virtual std::span<const ContactPair2D> GetContacts(EntityId id) const override
		{
			auto iter = m_ContactsByEntity.find(id);
			if (iter == m_ContactsByEntity.end())
			{
				return { };
			}

			return iter->second;
		}

		virtual bool IsColliding(EntityId id) const override
		{
			auto iter = m_ContactsByEntity.find(id);
			if (iter == m_ContactsByEntity.end())
			{
				return false;
			}

			return !iter->second.empty();
		}

		virtual bool IsColliding(EntityId idA, EntityId idB) const override
		{
			auto iter = m_ContactsByEntity.find(idA);
			if (iter == m_ContactsByEntity.end())
			{
				return false;
			}

			const auto& contacts = iter->second;
			return std::any_of(
				contacts.begin(),
				contacts.end(),
				[idA, idB](const ContactPair2D& pair)
			{
				return PairInvolves(pair, idA, idB);
			}
			);
		}

		virtual bool IsTriggering(EntityId id) const override
		{
			auto iter = m_ContactsByEntity.find(id);
			if (iter == m_ContactsByEntity.end())
			{
				return false;
			}

			const auto& contacts = iter->second;
			return std::any_of(
				contacts.begin(),
				contacts.end(),
				[](const ContactPair2D& pair)
			{
				return pair.IsTriggerPair;
			}
			);
		}

		virtual bool IsTriggering(EntityId idA, EntityId idB) const override
		{
			auto iter = m_ContactsByEntity.find(idA);
			if (iter == m_ContactsByEntity.end())
			{
				return false;
			}

			const auto& contacts = iter->second;

			return std::any_of(
				contacts.begin(),
				contacts.end(),
				[idA, idB](const ContactPair2D& pair)
			{
				return pair.IsTriggerPair && PairInvolves(pair, idA, idB);
			}
			);
		}

		virtual void Clear() override
		{
			m_ContactsByEntity.clear();
		}
	};
}
