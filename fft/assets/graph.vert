#version 330

in vec2 a_position;
uniform mat3 u_modelMatrix;

out vec2 v_position;

void main(){
    // Local Coordinates
    v_position  = a_position;
    
    // Global Coordinates
    vec2 m_position  = vec2(-1.0, -1.0) + (a_position * 2.0);
    gl_Position = vec4(m_position, 0.0, 1.0);
}
