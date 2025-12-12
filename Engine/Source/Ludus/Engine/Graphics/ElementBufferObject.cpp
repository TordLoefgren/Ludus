#include "pch.h"

#include <glad/glad.h>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Graphics/ElementBufferObject.h>

namespace Ludus::Engine::Graphics
{
	ElementBufferObject::ElementBufferObject(const void* data, unsigned int count)
		: m_Count(count), m_MaxCount(count)
	{
		glGenBuffers(1, &m_Handle);

		Bind();

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);

		Unbind();
	}

	ElementBufferObject::ElementBufferObject(unsigned int maxCount)
		: m_Count(0), m_MaxCount(maxCount)
	{
		glGenBuffers(1, &m_Handle);

		Bind();

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_MaxCount * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

		Unbind();
	}

	ElementBufferObject::~ElementBufferObject()
	{
		glDeleteBuffers(1, &m_Handle);
	}

	void ElementBufferObject::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
	}

	void ElementBufferObject::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void ElementBufferObject::Clear()
	{
		m_Count = 0;
	}

	void ElementBufferObject::AddDynamicData(const void* data, unsigned int count)
	{
		auto previousCount = m_Count;
		if (m_Count + count > m_MaxCount)
		{
			LUDUS_LOG_ERROR("Cannot add dynamic data. The element buffer is full.");

			return;
		}

		m_Count += count;

		Bind();

		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, previousCount * sizeof(uint32_t), count * sizeof(uint32_t), data);
	}

	unsigned int ElementBufferObject::GetCount() const { return m_Count; }

	unsigned int ElementBufferObject::GetSize() const { return m_Count * sizeof(unsigned int); }
}
