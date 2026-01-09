#pragma once

#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Persistance/ILoadArchive.h>
#include <Ludus/Tests/Persistance/TestNode.h>

namespace Ludus::Tests::Persistance
{
	using Ludus::Engine::Persistance::ILoadArchive;

	class TestLoadArchive final : public ILoadArchive
	{
	private:
		const TestNode& m_Root;
		std::vector<const TestNode*> m_NodeStack;

	public:
		TestLoadArchive(const TestNode& root)
			: m_Root(root)
		{
			m_NodeStack.push_back(&m_Root);
		}

		virtual void BeginObject(std::string_view key) override
		{
			const auto* parent = m_NodeStack.back();
			LUDUS_ASSERT(parent->NodeKind == TestNode::Kind::Object, "BeginObject must be used on an object.");

			auto& child = parent->Object.at(std::string(key));

			m_NodeStack.emplace_back(&child);
		}

		virtual void EndObject() override
		{
			LUDUS_ASSERT(m_NodeStack.size() > 1, "EndObject must not be called on the Root node.");
			m_NodeStack.pop_back();
		}

		virtual size_t BeginArray(std::string_view key) override
		{
			const auto keyString = std::string(key);

			const auto* parent = m_NodeStack.back();
			LUDUS_ASSERT(parent->Object.contains(keyString), "BeginArray must use a valid key.");

			auto& child = parent->Object.at(keyString);
			LUDUS_ASSERT(child.NodeKind == TestNode::Kind::Array, "BeginArray must be used on an object containing an array.");

			m_NodeStack.emplace_back(&child);

			return child.Array.size();
		}

		virtual void EndArray() override
		{
			LUDUS_ASSERT(m_NodeStack.size() > 1, "EndArray must not be called on the Root node.");
			m_NodeStack.pop_back();
		}

		virtual void BeginArrayElement(size_t index) override
		{
			const auto* parent = m_NodeStack.back();
			LUDUS_ASSERT(index < parent->Array.size(), "BeginArrayElement must use a valid index.");
			LUDUS_ASSERT(parent->NodeKind == TestNode::Kind::Array, "BeginArrayElement must be inside array.");

			auto& child = parent->Array[index];
			m_NodeStack.emplace_back(&child);
		}

		virtual void EndArrayElement() override
		{
			LUDUS_ASSERT(m_NodeStack.size() > 1, "EndArrayElement must not be called on the Root node.");
			m_NodeStack.pop_back();
		}

		virtual bool Has(std::string_view key) const override
		{
			auto& current = m_NodeStack.back();
			return current->Object.contains(std::string(key));
		}

		template<typename T>
		bool TryReadRaw(std::string_view key, T& outValue) const
		{
			auto& current = m_NodeStack.back();
			auto keyString = std::string(key);
			if (current->Object.contains(keyString))
			{
				auto* valuePtr = std::get_if<T>(&current->Object.at(keyString).Value);
				if (valuePtr)
				{
					outValue = *valuePtr;
					return true;
				}
			}

			return false;
		}

		virtual bool TryReadBool(std::string_view key, bool& outValue) const override { return TryReadRaw(key, outValue); };
		virtual bool TryReadDouble(std::string_view key, double& outValue) const override { return TryReadRaw(key, outValue); };
		virtual bool TryReadFloat(std::string_view key, float& outValue) const override { return TryReadRaw(key, outValue); };
		virtual bool TryReadInt(std::string_view key, int& outValue) const override { return TryReadRaw(key, outValue); };
		virtual bool TryReadString(std::string_view key, std::string& outValue) const override { return TryReadRaw(key, outValue); };
	};
}
