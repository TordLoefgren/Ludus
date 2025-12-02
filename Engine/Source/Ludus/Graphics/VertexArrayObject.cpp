#include <glad/glad.h>

#include <Ludus/Graphics/VertexArrayObject.h>

namespace Ludus::Graphics
{
	VertexArrayObject::VertexArrayObject()
		: m_Handle(0)
	{
		glGenVertexArrays(1, &m_Handle);
	}

	VertexArrayObject::~VertexArrayObject()
	{
		glDeleteVertexArrays(1, &m_Handle);
	}

	void VertexArrayObject::Bind() const
	{
		glBindVertexArray(m_Handle);
	}

	void VertexArrayObject::Unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArrayObject::SetVertexBuffer(const VertexBufferObject& vertexBuffer) const
	{
		Bind();
		vertexBuffer.Bind();

		const GLsizei stride = sizeof(Vertex);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, Position));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, Color));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, TexCoords));

		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 1, GL_INT, stride, (void*)offsetof(Vertex, Shape));

		glEnableVertexAttribArray(4);
		glVertexAttribIPointer(4, 1, GL_INT, stride, (void*)offsetof(Vertex, Fill));

		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 1, GL_INT, stride, (void*)offsetof(Vertex, TexSlot));
	}
}
