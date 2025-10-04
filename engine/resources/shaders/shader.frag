// FRAGMENT SHADER

#version 450 core

layout (location = 0) out vec4 o_Color; 

in vec4 v_Color;
in vec2 v_TexCoord;
in vec2 v_Position;
flat in int v_Shape;
flat in int v_TexSlot;

uniform sampler2D u_Textures[16];

void main()
{
    switch (v_Shape) 
    {
    case 0:
        // Quad.

        vec4 texColor = texture(u_Textures[v_TexSlot], v_TexCoord);
        o_Color = texColor * v_Color;
        
        break;
    case 1:
        // Circle
        
        float distance = length(v_Position);
        o_Color = (distance <= 0.5) ? v_Color : vec4(0,0,0,0);
        
        break;
    }
}