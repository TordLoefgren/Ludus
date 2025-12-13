#pragma once

namespace Ludus::Engine::Graphics
{
	struct Vertex
	{
		float WorldPosition[2];
		float LocalPosition[2];
		float Color[4];
		float TexCoords[2];
		int Shape;
		int Fill;
		int TexSlot;
	};
}
