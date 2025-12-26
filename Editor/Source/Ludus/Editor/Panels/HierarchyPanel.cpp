#include "pch.h"

#include <format>

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Panels/HierarchyPanel.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/UI/Context/InputContext.h>
#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Labels.h>
#include <Ludus/UI/Scope/MenuScope.h>
#include <Ludus/UI/Scope/PopupScope.h>
#include <Ludus/UI/Scope/TreeNodeScope.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Menu.h>
#include <Ludus/UI/Widgets/Selection.h>

namespace Ludus::Editor::Panels
{
	void HierarchyPanel::UpdateImpl(Ludus::Editor::Panels::PanelContext& context)
	{
		auto windowTitle = CreateWindowTitle("Hierarchy");
		if (Ludus::UI::Scope::WindowScope window(windowTitle.c_str(), &m_Open, Ludus::Editor::Core::Constants::PanelFlags); window)
		{
			auto& selection = context.EditorContext.State.Selection;

			for (auto& scene : context.SystemContext.SceneManager.ViewMutable())
			{
				const auto sceneHandle = scene.Handle;
				const auto sceneLabel = Ludus::UI::CreateLabel(std::format("Scene {}", sceneHandle), sceneHandle);

				if (Ludus::UI::Scope::TreeNodeScope treeNode(sceneLabel.c_str()); treeNode)
				{
					auto& ecs = scene.EntityComponentSystem;
					const auto& entities = ecs.View();

					if (Ludus::UI::Scope::PopupContextItemScope scenePopup; scenePopup)
					{
						if (Ludus::UI::Widgets::MenuItem("Create empty"))
						{
							auto handle = ecs.AddEntity();
							ecs.AttachTransform(handle);

							selection.SelectEntity(handle, sceneHandle);
						}

						auto hasSprite = false;

						if (Ludus::UI::Scope::MenuScope menu("2D Sprite"); menu)
						{
							Ludus::Engine::Graphics::Shape shape;
							std::string displayName;

							if (Ludus::UI::Widgets::MenuItem("Quad"))
							{
								shape = Ludus::Engine::Graphics::Shape::Quad;
								displayName = "Quad";
								hasSprite = true;
							}

							if (Ludus::UI::Widgets::MenuItem("Circle"))
							{
								shape = Ludus::Engine::Graphics::Shape::Circle;
								displayName = "Circle";
								hasSprite = true;
							}

							if (hasSprite)
							{
								auto handle = ecs.AddEntity();
								ecs.AttachTransform(handle);
								ecs.AttachSprite(handle, shape);

								selection.SelectEntity(handle, sceneHandle);
							}
						}

						if (Ludus::UI::Widgets::MenuItem("Set active scene"))
						{
							context.SystemContext.SceneManager.SetActiveScene(sceneHandle);
							selection.SelectScene(sceneHandle);
						}

						if (Ludus::UI::Widgets::MenuItem("Remove scene"))
						{
							context.SystemContext.SceneManager.RemoveScene(sceneHandle);
							selection.DeselectAll();
						}
					}

					for (auto& entity : entities)
					{
						const auto entityHandle = entity.Handle;
						const auto entityLabel = Ludus::UI::CreateLabel(std::format("Entity {}", entityHandle), entityHandle);

						if (Ludus::UI::Widgets::Selectable(entityLabel.c_str(), selection.IsSelected(entityHandle, sceneHandle)))
						{
							selection.SelectEntity(entityHandle, sceneHandle);
						}

						if (Ludus::UI::Scope::PopupContextItemScope contextItem; contextItem)
						{
							if (Ludus::UI::Widgets::MenuItem("Select"))
							{
								selection.SelectEntity(entityHandle, sceneHandle);
							}

							if (Ludus::UI::Widgets::MenuItem("Remove"))
							{
								ecs.DestroyEntity(entityHandle);
								if (selection.IsSelected(entityHandle, sceneHandle))
								{
									selection.DeselectAll();
								}
							}

							if (Ludus::UI::Scope::MenuScope componentMenu("Add Component"); componentMenu)
							{
								auto isComponentAdded = false;

								if (!ecs.Cameras.ContainsOwner(entityHandle) && Ludus::UI::Widgets::MenuItem("Camera 2D"))
								{
									ecs.AttachCamera(entityHandle);
									isComponentAdded = true;
								}

								if (!ecs.Colliders.ContainsOwner(entityHandle) && Ludus::UI::Widgets::MenuItem("Collider 2D"))
								{
									ecs.AttachCollider(entityHandle);
									isComponentAdded = true;
								}

								if (!ecs.RigidBodies.ContainsOwner(entityHandle) && Ludus::UI::Widgets::MenuItem("Rigid Body 2D"))
								{
									ecs.AttachRigidBody(entityHandle);
									isComponentAdded = true;
								}

								if (!ecs.Sprites.ContainsOwner(entityHandle) && Ludus::UI::Widgets::MenuItem("Sprite 2D"))
								{
									ecs.AttachSprite(entityHandle);
									isComponentAdded = true;
								}

								if (!ecs.Texts.ContainsOwner(entityHandle) && Ludus::UI::Widgets::MenuItem("Text 2D"))
								{
									ecs.AttachText(entityHandle, "");
									isComponentAdded = true;
								}

								if (isComponentAdded)
								{
									selection.SelectEntity(entityHandle, sceneHandle);
								}
							}
						}
					}
				}
			}

			auto flags = Ludus::UI::Flags::Popup::MouseButtonRight | Ludus::UI::Flags::Popup::NoOpenOverItems;
			if (Ludus::UI::Scope::PopupContextWindowScope contextWindow("HierarchyWindowContext", flags); contextWindow)
			{
				if (Ludus::UI::Widgets::MenuItem("Create scene"))
				{
					auto sceneHandle = context.SystemContext.SceneManager.AddScene();
					selection.SelectScene(sceneHandle);
				}
			}

			if (Ludus::UI::Context::InputContext::IsMouseClicked(Ludus::Engine::Platform::MouseButton::Left) &&
				Ludus::UI::Context::InputContext::IsWindowHovered(Ludus::UI::Flags::Hovered::AllowWhenBlockedByActiveItem) &&
				!Ludus::UI::Context::InputContext::IsAnyItemHovered())
			{
				selection.DeselectAll();
			}
		}
	}
}
