#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Persistance/ISaveArchive.h>
#include <Ludus/Tests/Persistance/TestNode.h>

namespace Ludus::Tests::Persistance
{
	using Ludus::Engine::Persistance::ISaveArchive;

	class TestSaveArchive final : public ISaveArchive
	{
	private:
		TestNode m_Root;
		std::vector<TestNode*> m_NodeStack;

		template<typename T>
		void WriteRaw(std::string_view key, T value)
		{
			auto* current = m_NodeStack.back();
			LUDUS_ASSERT(current->NodeKind == TestNode::Kind::Object, "Write must be inside object.");

			current->Object[std::string(key)] = TestNode::MakeValue(value);
		}

	public:
		TestSaveArchive()
		{
			m_Root = TestNode::MakeObject();
			m_NodeStack.push_back(&m_Root);
		}

		const TestNode& GetRoot() const { return m_Root; }

		virtual void BeginObject(std::string_view key) override
		{
			auto* parent = m_NodeStack.back();
			LUDUS_ASSERT(parent->NodeKind == TestNode::Kind::Object, "BeginObject must be inside object.");

			auto& child = parent->Object[std::string(key)];
			child = TestNode::MakeObject();

			m_NodeStack.emplace_back(&child);
		}

		virtual void EndObject() override
		{
			LUDUS_ASSERT(m_NodeStack.size() > 1, "EndObject must not be called on the Root node.");
			m_NodeStack.pop_back();
		}

		virtual void BeginArray(std::string_view key) override
		{
			auto* parent = m_NodeStack.back();
			LUDUS_ASSERT(parent->NodeKind == TestNode::Kind::Object, "BeginArray must be inside object.");

			auto& child = parent->Object[std::string(key)];
			child = TestNode::MakeArray();

			m_NodeStack.emplace_back(&child);
		}

		virtual void EndArray() override
		{
			LUDUS_ASSERT(m_NodeStack.size() > 1, "EndArray must not be called on the Root node.");
			m_NodeStack.pop_back();
		}

		virtual void BeginArrayElement() override
		{
			auto* parent = m_NodeStack.back();
			LUDUS_ASSERT(parent->NodeKind == TestNode::Kind::Array, "BeginArrayElement must be inside array.");

			parent->Array.push_back(TestNode::MakeObject());
			auto& child = parent->Array.back();

			m_NodeStack.emplace_back(&child);
		}

		virtual void EndArrayElement() override
		{
			LUDUS_ASSERT(m_NodeStack.size() > 1, "EndArrayElement must not be called on the Root node.");
			m_NodeStack.pop_back();
		}

		virtual void WriteBool(std::string_view key, bool value) override { WriteRaw(key, value); }
		virtual void WriteDouble(std::string_view key, double value) override { WriteRaw(key, value); }
		virtual void WriteFloat(std::string_view key, float value) override { WriteRaw(key, value); }
		virtual void WriteInt(std::string_view key, int value) override { WriteRaw(key, value); }
		virtual void WriteString(std::string_view key, std::string_view value) override { WriteRaw(key, value); }

		size_t GetStackSize() { return m_NodeStack.size(); }
	};
}
