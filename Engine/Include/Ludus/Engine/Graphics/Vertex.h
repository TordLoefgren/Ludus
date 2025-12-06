#pragma once

namespace Ludus::Engine::Graphics
{
	struct Vertex
	{
		float Position[2];
		float Color[4];
		float TexCoords[2];
		int Shape;
		int Fill;
		int TexSlot;
	};
}
