#include "pch.h"

#include <algorithm>

#include <Ludus/Engine/Graphics/GL.h>
#include <Ludus/Engine/Graphics/Renderer2D.h>

namespace Ludus::Engine::Graphics
{

#pragma region Constructor / Destructor

	static const glm::vec4 UnitQuad[4] = {
		{ -0.5f, -0.5f, 0.0f, 1.0f },
		{ 0.5f, -0.5f, 0.0f, 1.0f },
		{ 0.5f, 0.5f, 0.0f, 1.0f },
		{ -0.5f, 0.5f, 0.0f, 1.0f }
	};

	Renderer2D::Renderer2D(
		Shader& shader,
		std::filesystem::path fontPath,
		int maxTextures,
		int maxQuads
	) :
		m_VertexArray(VertexArrayObject()),
		m_VertexBuffer(VertexBufferObject(maxQuads * 4)),
		m_ElementBuffer(ElementBufferObject(maxQuads * 6)),
		m_Shader(shader),
		m_State { },
		m_MaxQuads(maxQuads),
		m_Textures(maxTextures, nullptr),
		m_MaxTextures(maxTextures),
		m_TexturesCount(0),
		m_Font(std::move(fontPath)),
		m_WhiteTexture(Texture::White())
	{
		m_VertexArray.SetVertexBuffer(m_VertexBuffer);
		m_Shader.SetSamplers(m_MaxTextures);
		m_Shader.SetStrokeThickness(1.0f);

		m_State.m_LineVertexCursor = 0;
		m_State.m_QuadVertexCursor = 0;
		m_State.m_QuadIndexCursor = 0;

		m_State.m_LineVertices.resize(maxQuads * 4);
		m_State.m_QuadVertices.resize(maxQuads * 4);
		m_State.m_QuadIndices.resize(maxQuads * 6);

		m_Textures[0] = &m_WhiteTexture;
		m_TexturesCount = 1;
	}

	Renderer2D::~Renderer2D()
	{}

#pragma endregion

#pragma region Scene Commands

	void Renderer2D::BeginScene(const Camera2D& camera)
	{
		m_State.m_LineVertexCursor = 0;
		m_State.m_QuadVertexCursor = 0;
		m_State.m_QuadIndexCursor = 0;

		m_TexturesCount = 1;
		m_Textures[0] = &m_WhiteTexture;

		m_Shader.Bind();

		glm::mat4 viewProjection = camera.GetViewProjection();
		m_Shader.SetUniformMat4fv("u_ViewProjection", viewProjection);
	}

	void Renderer2D::EndScene()
	{
		Flush();
	}

	void Renderer2D::DrawQuadInternal(const Ludus::Engine::Components::Transform2DComponent& transform, Color color, Texture* texture, TextureRegion textureRegion, RenderFill fill, RenderMode mode, RenderShape shape, bool flipU, bool flipV)
	{
		auto [r, g, b, a] = color;
		auto textureSlot = GetTextureSlot(texture);

		// Create a matrix to translate, rotate, and scale (TRS).
		auto matrix = glm::mat4(1.0f);
		matrix = glm::translate(matrix, glm::vec3(transform.Position.X, transform.Position.Y, 0.0f));
		matrix = glm::rotate(matrix, glm::radians(transform.Rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		matrix = glm::scale(matrix, glm::vec3(transform.Scale.X, transform.Scale.Y, 1.0f));

		glm::vec4 world[4];
		glm::vec2 local[4];
		for (int i = 0; i < 4; i++)
		{
			world[i] = matrix * UnitQuad[i];
			local[i] = glm::vec2(UnitQuad[i].x, UnitQuad[i].y);
		}

		auto left = textureRegion.Left;
		auto top = textureRegion.Top;
		auto right = textureRegion.Right;
		auto bottom = textureRegion.Bottom;

		if (flipU)
		{
			std::swap(left, right);
		}

		if (flipV)
		{
			std::swap(top, bottom);
		}

		glm::vec2 uv[4] = { { left, bottom }, { right, bottom }, { right, top }, { left, top } };

		m_State.m_QuadVertices[m_State.m_QuadVertexCursor + 0] = { { world[0].x, world[0].y }, { local[0].x, local[0].y }, { r, g, b, a }, uv[0].x, uv[0].y, static_cast<int>(fill), static_cast<int>(mode), static_cast<int>(shape), textureSlot };
		m_State.m_QuadVertices[m_State.m_QuadVertexCursor + 1] = { { world[1].x, world[1].y }, { local[1].x, local[1].y }, { r, g, b, a }, uv[1].x, uv[1].y, static_cast<int>(fill), static_cast<int>(mode), static_cast<int>(shape), textureSlot };
		m_State.m_QuadVertices[m_State.m_QuadVertexCursor + 2] = { { world[2].x, world[2].y }, { local[2].x, local[2].y }, { r, g, b, a }, uv[2].x, uv[2].y, static_cast<int>(fill), static_cast<int>(mode), static_cast<int>(shape), textureSlot };
		m_State.m_QuadVertices[m_State.m_QuadVertexCursor + 3] = { { world[3].x, world[3].y }, { local[3].x, local[3].y }, { r, g, b, a }, uv[3].x, uv[3].y, static_cast<int>(fill), static_cast<int>(mode), static_cast<int>(shape), textureSlot };

		auto offset = m_State.m_QuadVertexCursor;

		m_State.m_QuadIndices[m_State.m_QuadIndexCursor + 0] = uint32_t(offset + 0);
		m_State.m_QuadIndices[m_State.m_QuadIndexCursor + 1] = uint32_t(offset + 1);
		m_State.m_QuadIndices[m_State.m_QuadIndexCursor + 2] = uint32_t(offset + 2);
		m_State.m_QuadIndices[m_State.m_QuadIndexCursor + 3] = uint32_t(offset + 2);
		m_State.m_QuadIndices[m_State.m_QuadIndexCursor + 4] = uint32_t(offset + 3);
		m_State.m_QuadIndices[m_State.m_QuadIndexCursor + 5] = uint32_t(offset + 0);

		m_State.m_QuadVertexCursor += 4;
		m_State.m_QuadIndexCursor += 6;
	}

	void Renderer2D::DrawSprite(const Ludus::Engine::Components::Transform2DComponent& transform, Color color, Texture* texture, TextureRegion textureRegion, bool flipU, bool flipV)
	{
		if (WouldOverflowQuadBatch() || WouldExceedTextureSlots(texture))
		{
			Flush();
		}

		DrawQuadInternal(transform, color, texture, textureRegion, RenderFill::Fill, RenderMode::TexturedSprite, RenderShape::Quad, flipU, flipV);
	}

	void Renderer2D::DrawQuad(const Ludus::Engine::Components::Transform2DComponent& transform, Color color, Texture* texture, bool fill)
	{
		if (WouldOverflowQuadBatch() || WouldExceedTextureSlots(texture))
		{
			Flush();
		}

		DrawQuadInternal(transform, color, texture, { }, fill ? RenderFill::Fill : RenderFill::Stroke, RenderMode::SolidShape, RenderShape::Quad);
	}

	void Renderer2D::DrawCircle(const Ludus::Engine::Components::Transform2DComponent& transform, Color color, bool fill)
	{
		if (WouldOverflowQuadBatch())
		{
			Flush();
		}

		DrawQuadInternal(transform, color, nullptr, { }, fill ? RenderFill::Fill : RenderFill::Stroke, RenderMode::SolidShape, RenderShape::Circle);
	}

	void Renderer2D::DrawText(const Ludus::Engine::Components::Transform2DComponent& transform, std::string_view string, Color color, HorizontalTextAlignment horizontalTextAlignment)
	{
		auto position = transform.Position;

		const float scaleX = transform.Scale.X / static_cast<float>(m_Font.GetPixelSize());
		const float scaleY = transform.Scale.Y / static_cast<float>(m_Font.GetPixelSize());

		if (horizontalTextAlignment != HorizontalTextAlignment::Right)
		{
			const auto textWidthWorld = m_Font.MeasureTextWidth(string) * scaleX;

			if (horizontalTextAlignment == HorizontalTextAlignment::Center)
			{
				position.X -= 0.5f * textWidthWorld;
			}
			else
			{
				position.X -= textWidthWorld;
			}
		}

		for (char character : string)
		{
			const auto* glyph = m_Font.GetGlyph(character);
			if (!glyph)
			{
				continue;
			}

			auto texture = &const_cast<Glyph*>(glyph)->Texture;
			if (WouldOverflowQuadBatch() || WouldExceedTextureSlots(texture))
			{
				Flush();
			}

			const float x = position.X + glyph->BearingX * scaleX;
			const float y = position.Y + (glyph->BearingY - glyph->Rows) * scaleY;
			const float width = glyph->Width * scaleX;
			const float height = glyph->Rows * scaleY;

			const Ludus::Engine::Math::Vector2D quadCenter { x + width * 0.5f, y + height * 0.5f };
			const Ludus::Engine::Math::Vector2D quadScale { width, height };

			DrawQuadInternal(
				Ludus::Engine::Components::Transform2DComponent(transform.OwnerId, quadCenter, quadScale, transform.Rotation),
				color,
				texture,
				{ },
				RenderFill::Fill,
				RenderMode::TextGlyph,
				RenderShape::Quad,
				false,
				false
			);

			position.X += (glyph->Advance / 64.0f) * scaleX;
		}
	}

	void Renderer2D::DrawLine(float x1, float y1, float x2, float y2, Color color)
	{
		if (WouldOverflowLineBatch())
		{
			Flush();
		}

		auto [r, g, b, a] = color;
		auto textureSlot = 0;

		m_State.m_LineVertices[m_State.m_LineVertexCursor + 0] = { { x1, y1 }, { 0.0f, 0.0f }, { r, g, b, a }, { 0.0f, 0.0f }, static_cast<int>(RenderFill::Fill), static_cast<int>(RenderMode::Line), static_cast<int>(RenderShape::Quad), textureSlot };
		m_State.m_LineVertices[m_State.m_LineVertexCursor + 1] = { { x2, y2 }, { 0.0f, 0.0f }, { r, g, b, a }, { 1.0f, 0.0f }, static_cast<int>(RenderFill::Fill), static_cast<int>(RenderMode::Line), static_cast<int>(RenderShape::Quad), textureSlot };

		m_State.m_LineVertexCursor += 2;
	}

#pragma endregion

#pragma region Helpers

	void Renderer2D::Clear() const
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Renderer2D::Flush()
	{
		m_VertexArray.Bind();
		m_VertexBuffer.Bind();
		m_ElementBuffer.Bind();
		m_Shader.Bind();

		for (int i = 0; i < m_TexturesCount; i++)
		{
			m_Textures[i]->Bind(i);
		}

		if (m_State.m_QuadVertexCursor != 0 && m_State.m_QuadIndexCursor != 0)
		{
			m_VertexBuffer.AddDynamicData(m_State.m_QuadVertices.data(), m_State.m_QuadVertexCursor);
			m_ElementBuffer.AddDynamicData(m_State.m_QuadIndices.data(), m_State.m_QuadIndexCursor);

			glDrawElements(GL_TRIANGLES, m_State.m_QuadIndexCursor, GL_UNSIGNED_INT, nullptr);

			m_VertexBuffer.Clear();
			m_ElementBuffer.Clear();
		}

		if (m_State.m_LineVertexCursor != 0)
		{
			m_VertexBuffer.AddDynamicData(m_State.m_LineVertices.data(), m_State.m_LineVertexCursor);

			glDrawArrays(GL_LINES, 0, m_State.m_LineVertexCursor);

			m_VertexBuffer.Clear();
		}

		m_State.m_LineVertexCursor = 0;
		m_State.m_QuadVertexCursor = 0;
		m_State.m_QuadIndexCursor = 0;

		m_TexturesCount = 1;
		m_Textures[0] = &m_WhiteTexture;

		m_VertexArray.Unbind();
		m_VertexBuffer.Unbind();
		m_ElementBuffer.Unbind();
		m_Shader.Unbind();
	}

	bool Renderer2D::WouldOverflowQuadBatch(int vertexCount, int indexCount) const
	{
		return (m_State.m_QuadVertexCursor + vertexCount > static_cast<int>(m_State.m_QuadVertices.size())) ||
			(m_State.m_QuadIndexCursor + indexCount > static_cast<int>(m_State.m_QuadIndices.size()));
	}

	bool Renderer2D::WouldOverflowLineBatch(int vertexCount) const
	{
		return (m_State.m_LineVertexCursor + vertexCount > static_cast<int>(m_State.m_LineVertices.size()));
	}

	bool Renderer2D::WouldExceedTextureSlots(Texture* texture) const
	{
		if (!texture)
		{
			return false;
		}

		for (int i = 0; i < m_TexturesCount; i++)
		{
			if (m_Textures[i] == texture)
			{
				return false;
			}
		}

		return m_TexturesCount >= m_MaxTextures;
	}

	void Renderer2D::SetClearColor(Color color) const
	{
		glClearColor(color.R, color.G, color.B, color.A);
	}

	void Renderer2D::SetLineWidth(float width)
	{
		glLineWidth(width);
	}

	/// <summary>
	/// Sets the stroke thickness of rendered objects.
	/// </summary>
	/// <param name="thickness">The stroke thickness in pixels.</param>
	void Renderer2D::SetStrokeThickness(float thickness)
	{
		m_Shader.SetStrokeThickness(thickness);
	}

	int Renderer2D::GetQuadCount() const
	{
		return m_State.m_QuadVertexCursor / 4;
	}

	int Renderer2D::GetVertexCount() const
	{
		return m_State.m_QuadVertexCursor;
	}

	int Renderer2D::GetIndexCount() const
	{
		return m_State.m_QuadIndexCursor;
	}

	int Renderer2D::GetTextureSlot(Texture* texture)
	{
		if (!texture)
		{
			return 0;
		}

		for (int i = 0; i < m_TexturesCount; i++)
		{
			if (m_Textures[i] == texture)
			{
				return i;
			}
		}

		if (m_TexturesCount < m_MaxTextures)
		{
			m_Textures[m_TexturesCount] = texture;
			return m_TexturesCount++;
		}

		return 0;
	}

#pragma endregion
}
