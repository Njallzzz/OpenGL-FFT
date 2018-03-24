#pragma once
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

class Texture1D {
public:
	enum Texture1DSource {FILE, BUFFER};

	Texture1D(Texture1DSource source, const unsigned char* buffer, int width);
	~Texture1D();

	void Bind(GLint unit);

private:
	GLuint m_texture;
};

