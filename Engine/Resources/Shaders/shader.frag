// FRAGMENT SHADER

#version 450 core

layout (location = 0) out vec4 o_Color; 

in vec4 v_Color;
in vec2 v_TexCoord;
in vec2 v_Position;
flat in int v_Shape;
flat in int v_Fill;
flat in int v_TexSlot;

uniform sampler2D u_Textures[16];
uniform float u_StrokeThickness;

void main()
{
    if (v_Shape == 0)
    {
        // Quad.

        vec2 localPosition = abs(v_Position) - 0.5;

        float distance = length(max(localPosition, 0.0)) + min(max(localPosition.x, localPosition.y), 0.0);
        float distancePerPixel = fwidth(distance);

        if (v_Fill == 0) 
        {
            float alpha = 1.0 - smoothstep(0.0, distancePerPixel, distance);   
            vec4 tex = texture(u_Textures[v_TexSlot], v_TexCoord);
            
            o_Color = tex * vec4(v_Color.rgb, v_Color.a * alpha);
        } 
        else 
        {
            float halfWidthDistance = 0.5 * u_StrokeThickness * distancePerPixel;
            float alpha = smoothstep(halfWidthDistance + distancePerPixel, halfWidthDistance, abs(distance)); 
            
            o_Color = vec4(v_Color.rgb, v_Color.a * alpha);
        }
    }
    else
    {
        // Circle.
        
        // Rescale local coordinates to be in [-1, 1] to get a radius of 1.
        vec2 localPosition = v_Position * 2.0;

        float distance = 1.0 - length(localPosition);        
        float distancePerPixel = fwidth(distance);
        
        float alpha = smoothstep(0.0, distancePerPixel, distance); 

        if (v_Fill == 1) 
        {
            float halfWidth = 0.5 * u_StrokeThickness;  
            float halfWidthDistance = halfWidth * distancePerPixel;              

            alpha = smoothstep(halfWidthDistance + distancePerPixel, halfWidthDistance, abs(distance));
        }

        o_Color = vec4(v_Color.rgb, v_Color.a * alpha);
    }
}
