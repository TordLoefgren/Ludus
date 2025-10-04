#pragma once

#include "../core/utils.h"

class ElementBufferObject
{
private:
	unsigned int m_Handle { 0 };
	unsigned int m_Count { 0 };
	unsigned int m_MaxCount { 0 };

public:
	ElementBufferObject(const void* data, unsigned int count);
	ElementBufferObject(unsigned int maxCount);
	~ElementBufferObject();

	void Bind() const;
	void Unbind() const;

	void Clear();

	void AddDynamicData(const void* data, unsigned int count);

	unsigned int GetCount() const;
	unsigned int GetSize() const;
};
