#include "imguiAPI.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


//IMGUI_CHECKVERSION();
//ImGui::CreateContext();
//ImGuiIO& io = ImGui::GetIO(); (void)io;
//
//ImGui::StyleColorsDark();
//
//ImGui_ImplGlfw_InitForOpenGL(window, true);
//ImGui_ImplOpenGL3_Init("#version 450");

//while (!glfwWindowShouldClose(window))
//{
//	ImGui_ImplOpenGL3_NewFrame();
//	ImGui_ImplGlfw_NewFrame();
//	ImGui::NewFrame();

//	renderer.BeginScene(camera);
//	renderer.Clear();

//	const int columns = 5, rows = 5, padding = 5;

//	float cellWidth = (Width - 2 * padding - (columns - 1) * padding) / columns;
//	float cellHeight = (Height - 2 * padding - (rows - 1) * padding) / rows;

//	for (int y = 0; y < rows; y++)
//	{
//		for (int x = 0; x < columns; x++)
//		{
//			float px = padding + x * (cellWidth + padding);
//			float py = padding + y * (cellHeight + padding);

//			if (x == 2 || y == 2)
//			{
//				renderer.DrawQuad(px, py, cellWidth, cellHeight, (x + y) % 2 == 0 ? &texture3 : &texture4);
//				renderer.DrawQuad(px, py, cellWidth, cellHeight, Color(1.0f, 0.0f, 0.0f, 0.5f));
//			}
//			else
//			{
//				renderer.DrawQuad(px, py, cellWidth, cellHeight, (x + y) % 2 == 0 ? &texture3 : &texture4);
//			}
//		}
//	}

//	{
//		auto vertexCount = std::string("Vertex count: ") + std::to_string(renderer.GetVertexCount());
//		auto indexCount = std::string("Index count : ") + std::to_string(renderer.GetIndexCount());

//		ImGui::Text(vertexCount.c_str());
//		ImGui::Text(indexCount.c_str());
//		ImGui::SliderFloat("X", &position.x, -Width, Width);
//		ImGui::SliderFloat("Y", &position.y, -Height, Height);
//		ImGui::SliderFloat("Zoom", &zoom, 0.5f, 2.0f);
//		ImGui::SliderFloat("Rotation", &rotation, 0.0f, 360.0f);

//		if (ImGui::Button("Center"))
//		{
//			position.x = 0.0f;
//			position.y = 0.0f;
//			zoom = 1.0f;
//			rotation = 0.0f;
//		}
//	}

//	camera.SetPosition(position);
//	camera.SetZoom(zoom);
//	camera.SetRotation(rotation);

//	renderer.EndScene();

//	ImGui::Render();
//	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);

//	glfwSwapBuffers(window);
//	glfwPollEvents();
//}

//ImGui_ImplOpenGL3_Shutdown();
//ImGui_ImplGlfw_Shutdown();
//ImGui::DestroyContext();
