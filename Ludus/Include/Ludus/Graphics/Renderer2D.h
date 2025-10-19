#pragma once

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include <Ludus/Graphics/Camera2D.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/ElementBufferObject.h>
#include <Ludus/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Graphics/Shader.h>
#include <Ludus/Graphics/Text.h>
#include <Ludus/Graphics/Texture.h>
#include <Ludus/Graphics/VertexArrayObject.h>
#include <Ludus/Math/Transform2D.h>
#include <Ludus/Math/Vector2D.h>

namespace Ludus::Graphics
{
	struct RendererState
	{
		std::vector<Vertex> m_LineVertices;
		std::vector<Vertex> m_QuadVertices;
		std::vector<uint32_t> m_QuadIndices;

		int m_LineVertexCursor;
		int m_QuadVertexCursor;
		int m_QuadIndexCursor;
	};

	class Renderer2D
	{
	private:

		VertexArrayObject m_VertexArray;
		VertexBufferObject m_VertexBuffer;
		ElementBufferObject m_ElementBuffer;
		Shader& m_Shader;
		RendererState m_State;
		int m_MaxQuads;
		Texture m_WhiteTexture;
		Text m_Text;
		int m_MaxTextures;
		int m_TexturesCount;
		std::vector<const Texture*> m_Textures;

		void DrawQuadInternal(const Ludus::Math::Transform2D& transform, Color color, Texture* texture, int shape, int fill, bool flipU = false, bool flipV = false);

		bool WouldOverflow(int vertexCount, int indexCount) const;
		bool WouldExceedTextureSlots() const;

		int GetTextureSlot(Texture* texture);
		int GetIndexCount() const;
		int GetQuadCount() const;
		int GetVertexCount() const;

	public:

		Renderer2D(
			Shader& shader,
			int maxTextures = 16,
			int maxQuads = 1000
		);
		~Renderer2D();

		void BeginScene(Camera2D& camera);
		void EndScene();

		void DrawQuad(const Ludus::Math::Transform2D& transform, Color color = Colors::White, Texture* texture = nullptr, bool fill = true);
		void DrawLine(float x1, float y1, float x2, float y2, Color color = Colors::White);
		void DrawCircle(const Ludus::Math::Transform2D& transform, Color color = Colors::White, bool fill = true);
		void DrawText(const Ludus::Math::Transform2D& transform, std::string_view text, Color color = Colors::White, HorizontalTextAlignment horizontalAlignment = HorizontalTextAlignment::Left);

		void Clear() const;
		void Flush();
		void SetClearColor(Color color) const;
		void SetLineWidth(float width);
		void SetStrokeThickness(float thickness);
	};
}
