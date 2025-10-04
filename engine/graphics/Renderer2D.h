#pragma once

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "Camera2D.h"
#include "Color.h"
#include "ElementBufferObject.h"
#include "Shader.h"
#include "Text.h"
#include "Texture.h"
#include "Transform2D.h"
#include "VertexArrayObject.h"

struct Renderer2DState
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
	Shader m_Shader;
	Renderer2DState m_State;
	int m_MaxQuads;
	Texture m_WhiteTexture;
	Text m_Text;
	int m_MaxTextures;
	int m_TexturesCount;
	std::vector<const Texture*> m_Textures;

	void DrawQuadInternal(const Transform2D& transform, Color color, Texture* texture, int shape, bool flipU = false, bool flipV = false);

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

	void DrawQuad(const Transform2D& transform, Color color, Texture* texture = nullptr);
	void DrawLine(float x1, float y1, float x2, float y2, Color color);
	void DrawCircle(const Transform2D& transform, Color color);
	void DrawText(const Transform2D& transform, Color color, std::string_view string);

	void Clear() const;
	void Flush();
	void SetClearColor(Color color) const;
	void SetLineWidth(float width);
};
