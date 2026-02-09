#pragma once

namespace Ludus::Engine::Graphics
{
	class VertexBufferObject
	{
	private:
		unsigned int m_Handle;
		unsigned int m_Count;
		unsigned int m_MaxCount;

	public:
		VertexBufferObject(const void* data, unsigned int count);
		VertexBufferObject(unsigned int maxCount);
		~VertexBufferObject();

		void Bind() const;
		void Unbind() const;

		void Clear();

		void AddDynamicData(const void* data, unsigned int count);

		unsigned int GetCount() const;
		unsigned int GetSize() const;
	};
}
