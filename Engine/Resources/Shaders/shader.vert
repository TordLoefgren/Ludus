// VERTEX SHADER

#version 450 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in int a_Shape;
layout (location = 4) in int a_Fill;
layout (location = 5) in int a_TexSlot;

out vec4 v_Color;
out vec2 v_TexCoord;
out vec2 v_Position;
flat out int v_Shape;
flat out int v_Fill;
flat out int v_TexSlot;

uniform mat4 u_ViewProjection;

void main()
{
	gl_Position = u_ViewProjection * vec4(a_Position, 0.0, 1.0);
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_Position = a_TexCoord - 0.5;
	v_Shape = a_Shape;
	v_Fill = a_Fill;
	v_TexSlot = a_TexSlot;
}
