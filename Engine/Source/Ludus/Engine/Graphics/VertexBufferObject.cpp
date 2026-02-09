#include "pch.h"

#include <glad/glad.h>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Graphics/Vertex.h>
#include <Ludus/Engine/Graphics/VertexBufferObject.h>

namespace Ludus::Engine::Graphics
{
	VertexBufferObject::VertexBufferObject(const void* data, unsigned int count)
		: m_Handle(0), m_Count(count), m_MaxCount(count)
	{
		glGenBuffers(1, &m_Handle);

		Bind();

		glBufferData(GL_ARRAY_BUFFER, GetSize(), data, GL_STATIC_DRAW);

		Unbind();
	}

	VertexBufferObject::VertexBufferObject(unsigned int maxCount)
		: m_Count(0), m_MaxCount(maxCount)
	{
		glGenBuffers(1, &m_Handle);

		Bind();

		glBufferData(GL_ARRAY_BUFFER, m_MaxCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

		Unbind();
	}

	VertexBufferObject::~VertexBufferObject()
	{
		glDeleteBuffers(1, &m_Handle);
	}

	void VertexBufferObject::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
	}

	void VertexBufferObject::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VertexBufferObject::Clear()
	{
		m_Count = 0;
	}

	void VertexBufferObject::AddDynamicData(const void* data, unsigned int count)
	{
		auto previousCount = m_Count;
		if (m_Count + count > m_MaxCount)
		{
			LUDUS_LOG_ERROR("Cannot add dynamic data. The vertex buffer is full.");

			return;
		}

		m_Count += count;

		Bind();

		glBufferSubData(GL_ARRAY_BUFFER, previousCount * sizeof(Vertex), count * sizeof(Vertex), data);
	}

	unsigned int VertexBufferObject::GetCount() const { return m_Count; }

	unsigned int VertexBufferObject::GetSize() const { return m_Count * sizeof(Vertex); }
}
