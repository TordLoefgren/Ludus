#pragma once

#include <unordered_map>

#include <Ludus/Engine/Entity.h>
#include <Ludus/Physics/Narrowphase/ContactPair2D.h>
#include <Ludus/Physics/Queries/IPhysicsQueryCache2D.h>

namespace Ludus::Physics::Queries
{
	using EntityHandle = Ludus::Engine::EntityHandle;
	using ContactPair2D = Ludus::Physics::Narrowphase::ContactPair2D;

	class PhysicsQueryCache2D : public Ludus::Physics::Queries::IPhysicsQueryCache2D
	{
	private:
		std::vector<ContactPair2D> m_Contacts;
		std::unordered_map<EntityHandle, std::vector<ContactPair2D>> m_ContactsByEntity;

		static bool PairInvolves(const ContactPair2D& pair, EntityHandle a, EntityHandle b)
		{
			return (pair.EntityHandleA == a && pair.EntityHandleB == b) ||
				(pair.EntityHandleA == b && pair.EntityHandleB == a);
		}

	public:
		virtual void UpdateFromContacts(const std::vector <ContactPair2D> contactPairs)
		{
			m_ContactsByEntity.clear();
			m_ContactsByEntity.reserve(contactPairs.size() * 2);

			for (const auto& contact : contactPairs)
			{
				const EntityHandle handleA = contact.EntityHandleA;
				const EntityHandle handleB = contact.EntityHandleB;

				m_ContactsByEntity[handleA].push_back(contact);
				m_ContactsByEntity[handleB].push_back(contact);
			}
		}

		bool IsColliding(EntityHandle handle) const override
		{
			auto iter = m_ContactsByEntity.find(handle);
			if (iter == m_ContactsByEntity.end())
			{
				return false;
			}

			return !iter->second.empty();
		}

		bool IsColliding(EntityHandle handleA, EntityHandle handleB) const override
		{
			auto iter = m_ContactsByEntity.find(handleA);
			if (iter == m_ContactsByEntity.end())
			{
				return false;
			}

			const auto& contacts = iter->second;
			return std::any_of(
				contacts.begin(),
				contacts.end(),
				[handleA, handleB](const ContactPair2D& pair)
				{
					return PairInvolves(pair, handleA, handleB);
				}
			);
		}

		bool IsTriggering(EntityHandle handle) const override
		{
			auto iter = m_ContactsByEntity.find(handle);
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

		bool IsTriggering(EntityHandle handleA, EntityHandle handleB) const override
		{
			auto iter = m_ContactsByEntity.find(handleA);
			if (iter == m_ContactsByEntity.end())
			{
				return false;
			}

			const auto& contacts = iter->second;

			return std::any_of(
				contacts.begin(),
				contacts.end(),
				[handleA, handleB](const ContactPair2D& pair)
				{
					return pair.IsTriggerPair && PairInvolves(pair, handleA, handleB);
				}
			);
		}

		void Clear()
		{
			m_ContactsByEntity.clear();
		}
	};
}