#include <glad/glad.h>

#include <Ludus/Graphics/Renderer2D.h>
#include <Ludus/Math/Vector2D.h>

namespace Ludus::Graphics
{
	using Ludus::Math::Vector2D;

#pragma region Constructor / Desctructor

	static const glm::vec4 UnitQuad[4] = {
		{-0.5f, -0.5f, 0.0f, 1.0f},
		{ 0.5f, -0.5f, 0.0f, 1.0f},
		{ 0.5f,  0.5f, 0.0f, 1.0f},
		{-0.5f,  0.5f, 0.0f, 1.0f}
	};

	Renderer2D::Renderer2D(
		Shader& shader,
		int maxTextures,
		int maxQuads
	) :
		m_VertexArray(VertexArrayObject()),
		m_VertexBuffer(VertexBufferObject(maxQuads * 4)),
		m_ElementBuffer(ElementBufferObject(maxQuads * 6)),
		m_Shader(shader),
		m_State {},
		m_MaxQuads(maxQuads),
		m_Textures(maxTextures, nullptr),
		m_MaxTextures(maxTextures),
		m_TexturesCount(0),
		m_Text(Text()),
		m_WhiteTexture(Texture::White())
	{
		m_VertexArray.SetVertexBuffer(m_VertexBuffer);
		m_Shader.SetSamplers(m_MaxTextures);

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
	{ }

#pragma endregion

#pragma region Scene Commands

	void Renderer2D::BeginScene(Camera2D& camera)
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

	void Renderer2D::DrawQuadInternal(const Transform2D& transform, Color color, Texture* texture, int shape, bool flipU, bool flipV)
	{
		auto [r, g, b, a] = color;
		auto textureSlot = GetTextureSlot(texture);

		// Create a matrix to translate, rotate, and scale (TRS).
		auto matrix = glm::mat4(1.0f);
		matrix = glm::translate(matrix, glm::vec3(transform.Position.X, transform.Position.Y, 0.0f));
		matrix = glm::rotate(matrix, glm::radians(transform.Rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		matrix = glm::scale(matrix, glm::vec3(transform.Scale.X, transform.Scale.Y, 1.0f));

		glm::vec4 points[4];
		for (int i = 0; i < 4; i++)
		{
			points[i] = matrix * UnitQuad[i];
		}

		glm::vec2 uv[4] = { {0,0}, {1,0}, {1,1}, {0,1} };
		if (flipU)
		{
			for (auto& t : uv)
			{
				t.x = 1.0f - t.x;
			}
		}

		if (flipV)
		{
			for (auto& t : uv)
			{
				t.y = 1.0f - t.y;
			}
		}

		m_State.m_QuadVertices[m_State.m_QuadVertexCursor + 0] = { { points[0].x, points[0].y }, { r, g, b, a }, uv[0].x, uv[0].y, shape, textureSlot };
		m_State.m_QuadVertices[m_State.m_QuadVertexCursor + 1] = { { points[1].x, points[1].y }, { r, g, b, a }, uv[1].x, uv[1].y, shape, textureSlot };
		m_State.m_QuadVertices[m_State.m_QuadVertexCursor + 2] = { { points[2].x, points[2].y }, { r, g, b, a }, uv[2].x, uv[2].y, shape, textureSlot };
		m_State.m_QuadVertices[m_State.m_QuadVertexCursor + 3] = { { points[3].x, points[3].y }, { r, g, b, a }, uv[3].x, uv[3].y, shape, textureSlot };

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

	void Renderer2D::DrawQuad(const Transform2D& transform, Color color, Texture* texture)
	{
		if (WouldOverflow(4, 6))
		{
			Flush();
		}

		DrawQuadInternal(transform, color, texture, 0);
	}

	void Renderer2D::DrawCircle(const Transform2D& transform, Color color)
	{
		if (WouldOverflow(4, 6))
		{
			Flush();
		}

		DrawQuadInternal(transform, color, nullptr, 1);
	}

	void Renderer2D::DrawText(const Transform2D& transform, std::string_view string, Color color)
	{
		auto position = transform.Position;

		for (char character : string)
		{
			const Glyph* glyph = m_Text.GetGlyph(character);
			if (!glyph)
			{
				continue;
			}

			if (WouldOverflow(4, 6) || WouldExceedTextureSlots())
			{
				Flush();
			}

			const float x = position.X + glyph->BearingX * transform.Scale.X;
			const float y = position.Y + (glyph->BearingY - glyph->Rows) * transform.Scale.Y;
			const float width = glyph->Width * transform.Scale.X;
			const float height = glyph->Rows * transform.Scale.Y;

			const Vector2D quadCenter { x + width * 0.5f, y + height * 0.5f };
			const Vector2D quadScale { width, height };

			DrawQuadInternal(
				Transform2D(transform.OwnerHandle, quadCenter, quadScale, transform.Rotation),
				color,
				&const_cast<Glyph*>(glyph)->Texture,
				0,
				false,
				true // FreeType bitmaps are flipped vertically
			);

			position.X += (glyph->Advance / 64.0f) * transform.Scale.X;
		}
	}

	void Renderer2D::DrawLine(float x1, float y1, float x2, float y2, Color color)
	{
		if (WouldOverflow(2, 0))
		{
			Flush();
		}

		auto [r, g, b, a] = color;
		auto textureSlot = 0;

		m_State.m_LineVertices[m_State.m_LineVertexCursor + 0] = { { x1, y1 }, { r, g, b, a }, { 0.0f, 0.0f }, 0, textureSlot };
		m_State.m_LineVertices[m_State.m_LineVertexCursor + 1] = { { x2, y2 }, { r, g, b, a }, { 1.0f, 0.0f }, 0,  textureSlot };

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

	bool Renderer2D::WouldOverflow(int vertexCount, int indexCount) const
	{
		return (m_State.m_QuadVertexCursor + vertexCount > (int)m_State.m_QuadVertices.size()) ||
			(m_State.m_QuadIndexCursor + indexCount > (int)m_State.m_QuadIndices.size());
	}

	bool Renderer2D::WouldExceedTextureSlots() const
	{
		return m_TexturesCount >= m_MaxTextures;
	}

	void Renderer2D::SetClearColor(Color color) const
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void Renderer2D::SetLineWidth(float width)
	{
		glLineWidth(width);
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
