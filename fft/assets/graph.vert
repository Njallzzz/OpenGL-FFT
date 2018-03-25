#version 330

in vec2 a_position;
uniform mat3 u_modelMatrix;

uniform vec4 u_gridLines;
uniform int u_logFreq;

out vec2 v_position;

void main(){
    // Local Coordinates
    v_position  = a_position;
    
    // Global Coordinates
    float xAxis = a_position.x;
    if(u_logFreq == 1 && u_gridLines.w > 0) {
    	float frequency = 20000.0 * xAxis;
    	float upper = log2(frequency == 0 ? 1 : frequency);
    	xAxis = upper / 14.28771238;
    }
    vec2 m_position  = vec2(-1.0, -1.0) + (vec2(xAxis, a_position.y) * 2.0);
    gl_Position = vec4(m_position, 0.0, 1.0);
}
