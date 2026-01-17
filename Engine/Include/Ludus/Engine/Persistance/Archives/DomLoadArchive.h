#pragma once

#include <cstdint>
#include <format>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveAction.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveSeverity.h>
#include <Ludus/Engine/Persistance/Archives/DomNode.h>
#include <Ludus/Engine/Persistance/Archives/ILoadArchive.h>
#include <Ludus/Engine/Persistance/PersistanceException.h>

namespace Ludus::Engine::Persistance::Archives
{
	using Ludus::Engine::Persistance::Archives::ILoadArchive;

	class DomLoadArchive final : public ILoadArchive
	{
	private:
		const DomNode& m_Root;
		std::vector<std::string> m_PathStack;
		std::vector<const DomNode*> m_NodeStack;

		virtual std::string GetCurrentPath(std::string_view currentKey) const override
		{
			std::string path;
			for (const auto& string : m_PathStack)
			{
				path += string;
			}

			path += currentKey;

			return path;
		}

	public:
		DomLoadArchive(const DomNode& root, IArchiveDiagnostics* diagnostics = nullptr)
			: m_Root(root)
		{
			m_NodeStack.push_back(&m_Root);
			m_Diagnostics = diagnostics;
		}

		virtual bool TryBeginObject(std::string_view key) override
		{
			const auto* parent = m_NodeStack.back();
			LUDUS_ASSERT(std::holds_alternative<DomObject>(parent->NodeData), "TryBeginObject must be used on an object.");

			const auto keyString = std::string(key);
			const auto& object = std::get<DomObject>(parent->NodeData);
			if (!object.contains(keyString))
			{
				if (m_Diagnostics)
				{
					m_Diagnostics->Report(
						{
							ArchiveSeverity::Warn,
							ArchiveAction::ReadFailed,
							GetCurrentPath(keyString),
							std::string("Object did not contain a valid key: ") + keyString
						}
					);
				}

				return false;
			}

			auto& child = object.at(keyString);
			if (!std::holds_alternative<DomObject>(child.NodeData))
			{
				if (m_Diagnostics)
				{
					m_Diagnostics->Report(
						{
							ArchiveSeverity::Warn,
							ArchiveAction::ReadFailed,
							GetCurrentPath(keyString),
							std::string("Key was not an object: ") + keyString
						}
					);
				}

				return false;
			}

			m_NodeStack.emplace_back(&child);
			m_PathStack.emplace_back(keyString + ".");

			return true;
		}

		virtual void BeginObject(std::string_view key) override
		{
			const auto* parent = m_NodeStack.back();
			LUDUS_ASSERT(std::holds_alternative<DomObject>(parent->NodeData), "BeginObject must be used on an object.");

			const auto keyString = std::string(key);
			const auto& object = std::get<DomObject>(parent->NodeData);
			if (!object.contains(keyString))
			{
				throw PersistanceException(std::string("Object did not contain a valid key: ") + keyString);
			}

			auto& child = object.at(keyString);
			if (!std::holds_alternative<DomObject>(child.NodeData))
			{
				throw PersistanceException(std::string("Key was not an object: ") + keyString);
			}

			m_NodeStack.emplace_back(&child);
			m_PathStack.emplace_back(keyString + ".");
		}

		virtual void EndObject() override
		{
			LUDUS_ASSERT(m_NodeStack.size() > 1, "EndObject must not be called on the Root node.");
			m_NodeStack.pop_back();
			m_PathStack.pop_back();
		}

		virtual bool TryBeginArray(std::string_view key, size_t& outCount) override
		{
			const auto* parent = m_NodeStack.back();
			LUDUS_ASSERT(std::holds_alternative<DomObject>(parent->NodeData), "TryBeginArray must be used on an object.");

			const auto keyString = std::string(key);
			const auto& object = std::get<DomObject>(parent->NodeData);
			if (!object.contains(keyString))
			{
				if (m_Diagnostics)
				{
					m_Diagnostics->Report(
						{
							ArchiveSeverity::Warn,
							ArchiveAction::ReadFailed,
							GetCurrentPath(keyString),
							std::string("Object did not contain a valid key: ") + keyString
						}
					);
				}

				return false;
			}

			auto& child = object.at(keyString);
			if (!std::holds_alternative<DomArray>(child.NodeData))
			{
				if (m_Diagnostics)
				{
					m_Diagnostics->Report(
						{
							ArchiveSeverity::Warn,
							ArchiveAction::ReadFailed,
							GetCurrentPath(keyString),
							std::string("Key was not an array object: ") + keyString
						}
					);
				}

				return false;
			}

			m_NodeStack.emplace_back(&child);
			m_PathStack.emplace_back(keyString);

			outCount = std::get<DomArray>(child.NodeData).size();
			return true;
		}

		virtual size_t BeginArray(std::string_view key) override
		{
			const auto* parent = m_NodeStack.back();
			LUDUS_ASSERT(std::holds_alternative<DomObject>(parent->NodeData), "BeginArray must be used on an object.");

			const auto keyString = std::string(key);
			const auto& object = std::get<DomObject>(parent->NodeData);
			if (!object.contains(keyString))
			{
				throw PersistanceException(std::string("Object did not contain a valid key: ") + keyString);
			}

			auto& child = object.at(keyString);
			if (!std::holds_alternative<DomArray>(child.NodeData))
			{
				throw PersistanceException(std::string("Key was not an array object: ") + keyString);
			}

			m_NodeStack.emplace_back(&child);
			m_PathStack.emplace_back(keyString);

			return std::get<DomArray>(child.NodeData).size();
		}

		virtual void EndArray() override
		{
			LUDUS_ASSERT(m_NodeStack.size() > 1, "EndArray must not be called on the Root node.");
			m_NodeStack.pop_back();
			m_PathStack.pop_back();
		}

		virtual void BeginArrayElement(size_t index) override
		{
			const auto* parent = m_NodeStack.back();
			LUDUS_ASSERT(std::holds_alternative<DomArray>(parent->NodeData), "BeginArrayElement must be inside array.");

			const auto& array = std::get<DomArray>(parent->NodeData);
			LUDUS_ASSERT(index < array.size(), "BeginArrayElement must use a valid index.");

			auto& child = array[index];
			m_NodeStack.emplace_back(&child);
			m_PathStack.emplace_back(std::format("[{}].", index));
		}

		virtual void EndArrayElement() override
		{
			LUDUS_ASSERT(m_NodeStack.size() > 1, "EndArrayElement must not be called on the Root node.");
			m_NodeStack.pop_back();
			m_PathStack.pop_back();
		}

		virtual bool Has(std::string_view key) const override
		{
			const auto* current = m_NodeStack.back();
			LUDUS_ASSERT(std::holds_alternative<DomObject>(current->NodeData), "Has must be used on an object.");

			return std::get<DomObject>(current->NodeData).contains(std::string(key));
		}

		template<typename T>
			requires (IsSupportedValue<T>)
		void ReadRaw(std::string_view key, T& outValue) const
		{
			const auto* current = m_NodeStack.back();
			LUDUS_ASSERT(std::holds_alternative<DomObject>(current->NodeData), "Read must be used on an object.");

			const auto keyString = std::string(key);
			const auto& object = std::get<DomObject>(current->NodeData);
			if (object.contains(keyString))
			{
				const auto& child = object.at(keyString);
				if (!std::holds_alternative<DomValue>(child.NodeData))
				{
					throw Ludus::Engine::Persistance::PersistanceException(std::string("Key was not a value: ") + keyString);
				}

				const auto& value = std::get<DomValue>(child.NodeData);
				if (auto* valuePtr = std::get_if<std::decay_t<T>>(&value))
				{
					outValue = *valuePtr;
				}
				else
				{
					throw Ludus::Engine::Persistance::PersistanceException(std::string("Object did not contain a valid value for key: ") + keyString);
				}
			}
			else
			{
				throw Ludus::Engine::Persistance::PersistanceException(std::string("Object did not contain a valid key: ") + keyString);
			}
		}

		template<typename T>
			requires (IsSupportedValue<T>)
		bool TryReadRaw(std::string_view key, T& outValue) const
		{
			const auto* current = m_NodeStack.back();
			LUDUS_ASSERT(std::holds_alternative<DomObject>(current->NodeData), "TryRead must be used on an object.");

			const auto& object = std::get<DomObject>(current->NodeData);
			const auto keyString = std::string(key);
			if (!object.contains(keyString))
			{
				if (m_Diagnostics)
				{
					m_Diagnostics->Report(
						{
							ArchiveSeverity::Warn,
							ArchiveAction::ReadFailed,
							GetCurrentPath(keyString),
							std::string("Object did not contain a valid key: ") + keyString
						}
					);
				}

				return false;
			}

			const auto& child = object.at(keyString);
			if (!std::holds_alternative<DomValue>(child.NodeData))
			{
				if (m_Diagnostics)
				{
					m_Diagnostics->Report(
						{
							ArchiveSeverity::Warn,
							ArchiveAction::ReadFailed,
							GetCurrentPath(keyString),
							std::string("Key was not a value: ") + keyString
						}
					);
				}

				return false;
			}

			const auto& value = std::get<DomValue>(child.NodeData);

			auto* valuePtr = std::get_if<std::decay_t<T>>(&value);
			if (!valuePtr)
			{
				if (m_Diagnostics)
				{
					m_Diagnostics->Report(
						{
							ArchiveSeverity::Warn,
							ArchiveAction::ReadFailed,
							GetCurrentPath(keyString),
							std::string("Object did not contain a valid type for key: ") + keyString
						}
					);
				}

				return false;
			}

			outValue = *valuePtr;
			return true;
		}

		virtual void ReadBool(std::string_view key, bool& outValue) const override { ReadRaw(key, outValue); };
		virtual void ReadDouble(std::string_view key, double& outValue) const override { ReadRaw(key, outValue); };
		virtual void ReadFloat(std::string_view key, float& outValue) const override { ReadRaw(key, outValue); };
		virtual void ReadInt(std::string_view key, int& outValue) const override { ReadRaw(key, outValue); };
		virtual void ReadString(std::string_view key, std::string& outValue) const override { ReadRaw(key, outValue); };
		virtual void ReadUint8(std::string_view key, uint8_t& outValue) const override { ReadRaw(key, outValue); };
		virtual void ReadUint32(std::string_view key, uint32_t& outValue) const override { ReadRaw(key, outValue); };

		virtual bool TryReadBool(std::string_view key, bool& outValue) const override { return TryReadRaw(key, outValue); };
		virtual bool TryReadDouble(std::string_view key, double& outValue) const override { return TryReadRaw(key, outValue); };
		virtual bool TryReadFloat(std::string_view key, float& outValue) const override { return TryReadRaw(key, outValue); };
		virtual bool TryReadInt(std::string_view key, int& outValue) const override { return TryReadRaw(key, outValue); };
		virtual bool TryReadString(std::string_view key, std::string& outValue) const override { return TryReadRaw(key, outValue); };
		virtual bool TryReadUint8(std::string_view key, uint8_t& outValue) const override { return TryReadRaw(key, outValue); };
		virtual bool TryReadUint32(std::string_view key, uint32_t& outValue) const override { return TryReadRaw(key, outValue); };

		size_t GetStackSize() const { return m_NodeStack.size(); }
	};
}
