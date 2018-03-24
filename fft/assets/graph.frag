#version 330

uniform float u_lowColorGradient;
uniform float u_highColorGradient;

uniform sampler1D u_colorTexture;
uniform sampler1D u_dataTexture;

uniform int u_logFreq;

in vec2 v_position;
out vec4 o_color;

void main() {
    float posx;
    if(u_logFreq == 1)
        posx = pow(v_position.x, 2);
    else
        posx = v_position.x;
    float data = texture(u_dataTexture, posx)[0];

    if(data < v_position.y) {
        discard;
    } else {
        float color = v_position.y;

        float colorMapOffset = clamp(color, 0.1, 0.9);
        vec4 colorMapSample = texture(u_colorTexture, colorMapOffset);
        o_color = vec4(colorMapSample.xyz, 1.0);
    }

}