#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Persistance/Archives/DomNode.h>
#include <Ludus/Engine/Persistance/Archives/ISaveArchive.h>

namespace Ludus::Engine::Persistance::Archives
{
	using Ludus::Engine::Persistance::Archives::ISaveArchive;

	class DomSaveArchive final : public ISaveArchive
	{
	private:
		DomNode m_Root;
		std::vector<DomNode*> m_NodeStack;

		template<typename T>
		void WriteRaw(std::string_view key, T&& value)
		{
			auto* current = m_NodeStack.back();
			LUDUS_ASSERT(std::holds_alternative<DomObject>(current->NodeData), "Write must be used on an object.");

			auto& child = std::get<DomObject>(current->NodeData);
			child[std::string(key)] = MakeValueNode(std::forward<T>(value));
		}

	public:
		DomSaveArchive(IArchiveDiagnostics* diagnostics = nullptr)
		{
			m_Root = MakeObjectNode();
			m_NodeStack.push_back(&m_Root);
			m_Diagnostics = diagnostics;
		}

		const DomNode& GetRoot() const { return m_Root; }

		virtual void BeginObject(std::string_view key) override
		{
			auto* parent = m_NodeStack.back();
			LUDUS_ASSERT(std::holds_alternative<DomObject>(parent->NodeData), "BeginObject must be used on an object.");

			auto& child = std::get<DomObject>(parent->NodeData)[std::string(key)];
			child = MakeObjectNode();

			m_NodeStack.emplace_back(&child);
		}

		virtual void EndObject() override
		{
			LUDUS_ASSERT(m_NodeStack.size() > 1, "EndObject must not be called on the Root node.");
			LUDUS_ASSERT(std::holds_alternative<DomObject>(m_NodeStack.back()->NodeData), "EndObject must be used on an object.");
			LUDUS_ASSERT(
				m_NodeStack.size() >= 2 && std::holds_alternative<DomObject>(m_NodeStack[m_NodeStack.size() - 2]->NodeData),
				"EndObject must be used inside an object."
			);

			m_NodeStack.pop_back();
		}

		virtual void BeginArray(std::string_view key) override
		{
			auto* parent = m_NodeStack.back();
			LUDUS_ASSERT(std::holds_alternative<DomObject>(parent->NodeData), "BeginArray must be used on an object.");

			auto& child = std::get<DomObject>(parent->NodeData)[std::string(key)];
			child = MakeArrayNode();

			m_NodeStack.emplace_back(&child);
		}

		virtual void EndArray() override
		{
			LUDUS_ASSERT(m_NodeStack.size() > 1, "EndArray must not be called on the Root node.");
			LUDUS_ASSERT(std::holds_alternative<DomArray>(m_NodeStack.back()->NodeData), "EndArray must be used on an array.");
			LUDUS_ASSERT(
				m_NodeStack.size() >= 2 && std::holds_alternative<DomObject>(m_NodeStack[m_NodeStack.size() - 2]->NodeData),
				"EndArray must be used inside an object."
			);

			m_NodeStack.pop_back();
		}

		virtual void BeginArrayElement(size_t index) override
		{
			auto* parent = m_NodeStack.back();
			LUDUS_ASSERT(std::holds_alternative<DomArray>(parent->NodeData), "BeginArrayElement must be used on an array.");

			auto& arrayNode = std::get<DomArray>(parent->NodeData);
			LUDUS_ASSERT(index == arrayNode.size(), "BeginArrayElement index must append at end.");

			arrayNode.push_back(MakeObjectNode());
			m_NodeStack.emplace_back(&arrayNode.back());
		}

		virtual void BeginArrayElement() override
		{
			auto* parent = m_NodeStack.back();
			LUDUS_ASSERT(std::holds_alternative<DomArray>(parent->NodeData), "BeginArrayElement must be used on an array.");

			auto& arrayNode = std::get<DomArray>(parent->NodeData);

			BeginArrayElement(arrayNode.size());
		}

		virtual void EndArrayElement() override
		{
			LUDUS_ASSERT(m_NodeStack.size() > 1, "EndArrayElement must not be called on the Root node.");
			LUDUS_ASSERT(std::holds_alternative<DomObject>(m_NodeStack.back()->NodeData), "EndArrayElement must be used on an array element object.");
			LUDUS_ASSERT(
				m_NodeStack.size() >= 2 && std::holds_alternative<DomArray>(m_NodeStack[m_NodeStack.size() - 2]->NodeData),
				"EndArrayElement must be used on an array element object."
			);

			m_NodeStack.pop_back();
		}

		virtual void WriteBool(std::string_view key, bool value) override { WriteRaw(key, value); }
		virtual void WriteDouble(std::string_view key, double value) override { WriteRaw(key, value); }
		virtual void WriteFloat(std::string_view key, float value) override { WriteRaw(key, value); }
		virtual void WriteInt(std::string_view key, int value) override { WriteRaw(key, value); }
		virtual void WriteString(std::string_view key, std::string_view value) override { WriteRaw(key, std::string(value)); }
		virtual void WriteUint8(std::string_view key, uint8_t value) override { WriteRaw(key, value); }
		virtual void WriteUint32(std::string_view key, uint32_t value) override { WriteRaw(key, value); }

		size_t GetStackSize() const { return m_NodeStack.size(); }
	};
}
