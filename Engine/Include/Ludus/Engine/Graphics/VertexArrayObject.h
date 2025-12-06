#pragma once

#include <Ludus/Engine/Graphics/VertexBufferObject.h>

namespace Ludus::Engine::Graphics
{
	class VertexArrayObject
	{
	private:
		unsigned int m_Handle;

	public:
		VertexArrayObject();
		~VertexArrayObject();

		void Bind() const;
		void Unbind() const;

		void SetVertexBuffer(const VertexBufferObject& vertexBuffer) const;

		unsigned int GetCount() const;
	};
}
