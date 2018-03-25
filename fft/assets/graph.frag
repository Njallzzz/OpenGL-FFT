#version 430

uniform float u_lowColorGradient;
uniform float u_highColorGradient;

uniform sampler1D u_colorTexture;
uniform sampler1D u_dataTexture;

uniform int u_dataSize;
layout(binding = 2) buffer b_dataBuffer
{
    float dataObj[];
};

uniform vec4 u_gridLines;
uniform int u_logFreq;

in vec2 v_position;
out vec4 o_color;

void main() {
    if(u_gridLines.w > 0.0) {
        o_color = u_gridLines;
        return;
    }

    float posx;
    if(u_logFreq == 1) {
        posx = pow(2, v_position.x * 14.28771238) / 20000.0;
    } else {
        posx = v_position.x;
    }

    float exact = float(u_dataSize) * posx;

    float floorFrac;
    float delta = modf(exact, floorFrac);
    int floorIndex = int(floorFrac);

    int topIndex = u_dataSize == floorIndex ? floorIndex : floorIndex + 1;

    float floorVal = dataObj[floorIndex];
    float topVal = dataObj[topIndex];

    float data = (floorVal * (1.0 - delta)) + (topVal * delta);

    if(data < v_position.y) {
        discard;
    } else {
        float color = v_position.y;

        float colorMapOffset = clamp(color, 0.1, 0.9);
        vec4 colorMapSample = texture(u_colorTexture, colorMapOffset);
        o_color = vec4(colorMapSample.xyz, 1.0);
    }

}