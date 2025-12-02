#pragma once

#include <functional>
#include <string>

#include <imgui/imgui.h>

namespace Ludus::UI::Containers
{
	struct Window
	{
	private:
		bool m_Active;

	public:
		explicit Window(const char* title, bool* open, ImGuiWindowFlags flags = ImGuiWindowFlags_None)
		{
			m_Active = ImGui::Begin(title, open, flags);
		}

		~Window()
		{
			ImGui::End();
		}

		operator bool() { return m_Active; }
	};

	struct TreeNode
	{
	private:
		bool m_Open;

	public:
		explicit TreeNode(const char* label, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed)
		{

			m_Open = ImGui::TreeNodeEx(label, flags);
		}

		~TreeNode()
		{
			if (m_Open)
			{
				ImGui::TreePop();
			}
		}

		operator bool() { return m_Open; }
	};

	struct Table
	{
	private:
		bool m_Open;

	public:
		explicit Table(const char* label, int columns, ImGuiTableFlags flags = ImGuiTableFlags_None)
		{
			m_Open = ImGui::BeginTable(label, columns, flags);
		}

		~Table()
		{
			if (m_Open)
			{
				ImGui::EndTable();
			}
		}

		operator bool() { return m_Open; }
	};

	struct ListBox
	{
	private:
		bool m_Open;

	public:
		explicit ListBox(const char* label)
		{
			m_Open = ImGui::BeginListBox(label);
		}

		~ListBox()
		{
			if (m_Open)
			{
				ImGui::EndListBox();

			}
		}

		operator bool() { return m_Open; }
	};

	inline void Selectable(const char* label) { ImGui::Selectable(label); }

	inline void TableNextRow() { ImGui::TableNextRow(); }

	inline void TableNextRowFirstColumn()
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
	}

	inline void TableSetColumnIndex(int index) { ImGui::TableSetColumnIndex(index); }
}
