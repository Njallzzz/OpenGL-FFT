#version 330

uniform vec4 u_backgroundColor;

// Color Region
uniform float u_lowColorGradient;
uniform float u_highColorGradient;

uniform float u_dataFloor;

uniform sampler2D u_colorMapTexture;
uniform sampler2D u_dataMapTexture;

in vec2 v_position;
out vec4 o_color;

void main() {
    vec2 g_position = (vec2(1.0) + v_position) / 2.0;
    float data = texture2D(u_dataMapTexture, g_position)[0];

    data -= u_dataFloor;
    if(data < g_position.y) {
        o_color = u_backgroundColor;
    } else {
        float color = g_position.y;
        
        if(g_position.y < u_lowColorGradient) {
            color = 0.0f;
        } else if(g_position.y > u_highColorGradient) {
            color = 1.0f;
        } else {
            color -= u_lowColorGradient;
            color /= u_highColorGradient - u_lowColorGradient;
        }
        
        vec2 map = vec2(clamp(color, 0.1, 0.9), 0.0);
        o_color = texture2D(u_colorMapTexture, map);
    }
}