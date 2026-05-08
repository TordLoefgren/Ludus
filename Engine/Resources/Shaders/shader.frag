// FRAGMENT SHADER

#version 450 core

layout (location = 0) out vec4 o_Color; 

in vec4 v_Color;
in vec2 v_TexCoord;
in vec2 v_LocalPosition;
flat in int v_Fill;
flat in int v_Mode;
flat in int v_Shape;
flat in int v_TexSlot;

uniform sampler2D u_Textures[16];
uniform float u_StrokeThickness;

const int FILL_FILL = 0;
const int FILL_STROKE = 1;

const int MODE_SOLID_SHAPE = 0;
const int MODE_TEXTURED_SPRITE = 1;
const int MODE_TEXT_GLYPH = 2;
const int MODE_LINE = 3;

const int SHAPE_QUAD = 0;
const int SHAPE_CIRCLE = 1;

void main()
{
    if (v_Mode == MODE_SOLID_SHAPE)
    {
        if (v_Shape == SHAPE_QUAD)
        {
            vec2 localPosition = abs(v_LocalPosition) - 0.5;
            float dist = length(max(localPosition, 0.0)) + min(max(localPosition.x, localPosition.y), 0.0);
            float distancePerPixel = fwidth(dist);

            float alpha;
            if (v_Fill == FILL_FILL)
            {
                alpha = 1.0 - smoothstep(0.0, distancePerPixel, dist);
            }
            else
            {
                float strokeWidthDistance = u_StrokeThickness * distancePerPixel;

                float outerAlpha = smoothstep(0.0, distancePerPixel, -dist);
                float innerAlpha = 1.0 - smoothstep(strokeWidthDistance, strokeWidthDistance + distancePerPixel, -dist);
                alpha = outerAlpha * innerAlpha;
            }

            o_Color = vec4(v_Color.rgb, v_Color.a * alpha);

            return;
        }

        if (v_Shape == SHAPE_CIRCLE)
        {
            // Rescale local coordinates to be in [-1, 1] to get a radius of 1.
            vec2 localPosition = v_LocalPosition * 2.0;
            float dist = 1.0 - length(localPosition);
            float distancePerPixel = fwidth(dist);

            float alpha;
            if (v_Fill == FILL_FILL)
            {
                alpha = smoothstep(0.0, distancePerPixel, dist);
            }
            else
            {
                float strokeWidthDistance = u_StrokeThickness * distancePerPixel;

                float outerAlpha = smoothstep(0.0, distancePerPixel, dist);
                float innerAlpha = 1.0 - smoothstep(
                    strokeWidthDistance,
                    strokeWidthDistance + distancePerPixel,
                    dist
                );

                alpha = outerAlpha * innerAlpha;
            }

            o_Color = vec4(v_Color.rgb, v_Color.a * alpha);

            return;
        }
    }

    if (v_Mode == MODE_TEXTURED_SPRITE)
    {
        vec4 tex = texture(u_Textures[v_TexSlot], v_TexCoord);
        o_Color = tex * v_Color;

        return;
    }

    if (v_Mode == MODE_TEXT_GLYPH)
    {
        float coverage = texture(u_Textures[v_TexSlot], v_TexCoord).a;
        o_Color = vec4(v_Color.rgb, v_Color.a * coverage);

        return;
    }

    if (v_Mode == MODE_LINE)
    {
        o_Color = v_Color;
        
        return;
    }
}
