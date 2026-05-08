#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Graphics/Camera2D.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/ElementBufferObject.h>
#include <Ludus/Engine/Graphics/Font.h>
#include <Ludus/Engine/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Engine/Graphics/Shader.h>
#include <Ludus/Engine/Graphics/Texture.h>
#include <Ludus/Engine/Graphics/TextureRegion.h>
#include <Ludus/Engine/Graphics/Vertex.h>
#include <Ludus/Engine/Graphics/VertexArrayObject.h>
#include <Ludus/Engine/Math/Vector2D.h>

namespace Ludus::Engine::Graphics
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
		enum class RenderFill : int
		{
			Fill = 0,
			Stroke = 1
		};

		enum class RenderMode : int
		{
			SolidShape = 0,
			TexturedSprite = 1,
			TextGlyph = 2,
			Line = 3
		};

		enum class RenderShape : int
		{
			Quad = 0,
			Circle = 1
		};

		VertexArrayObject m_VertexArray;
		VertexBufferObject m_VertexBuffer;
		ElementBufferObject m_ElementBuffer;
		Shader& m_Shader;
		RendererState m_State;
		int m_MaxQuads;
		Texture m_WhiteTexture;
		Font m_Font;
		int m_MaxTextures;
		int m_TexturesCount;
		std::vector<const Texture*> m_Textures;

		void DrawQuadInternal(
			const Ludus::Engine::Components::Transform2DComponent& transform,
			Color color,
			Texture* texture,
			TextureRegion textureRegion,
			RenderFill fill,
			RenderMode mode,
			RenderShape shape,
			bool flipU = false,
			bool flipV = false
		);

		bool WouldOverflowQuadBatch(int vertexCount = 4, int indexCount = 6) const;
		bool WouldOverflowLineBatch(int vertexCount = 2) const;
		bool WouldExceedTextureSlots(Texture* texture) const;

		int GetTextureSlot(Texture* texture);
		int GetIndexCount() const;
		int GetQuadCount() const;
		int GetVertexCount() const;

	public:
		Renderer2D(
			Shader& shader,
			std::filesystem::path fontPath,
			int maxTextures = 16,
			int maxQuads = 1000
		);
		~Renderer2D();

		void BeginScene(const Camera2D& camera);
		void EndScene();

		void DrawSprite(const Ludus::Engine::Components::Transform2DComponent& transform, Color color = Colors::White, Texture* texture = nullptr, TextureRegion textureRegion = { }, bool flipU = false, bool flipV = false);
		void DrawQuad(const Ludus::Engine::Components::Transform2DComponent& transform, Color color = Colors::White, Texture* texture = nullptr, bool fill = true);
		void DrawCircle(const Ludus::Engine::Components::Transform2DComponent& transform, Color color = Colors::White, bool fill = true);
		void DrawText(const Ludus::Engine::Components::Transform2DComponent& transform, std::string_view text, Color color = Colors::White, HorizontalTextAlignment horizontalTextAlignment = HorizontalTextAlignment::Left);
		void DrawLine(float x1, float y1, float x2, float y2, Color color = Colors::White);

		void Clear() const;
		void Flush();
		void SetClearColor(Color color) const;
		void SetLineWidth(float width);
		void SetStrokeThickness(float thickness);
	};
}
