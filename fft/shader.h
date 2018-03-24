#ifndef _SHADER_H_
#define _SHADER_H_

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <streambuf>

using std::ostream;
using std::endl;
using std::vector;
using std::ifstream;
using std::string;
using std::istreambuf_iterator;

class Shader {
public:
	Shader(	const char *vertexShaderFile,
			const char *fragmentShaderFile);
	~Shader();

	int compile(ostream *stream);
	void use();

	GLint getVertexPointer();
	GLint getColorTexturePointer();
	GLint getDataTexturePointer();

	void setLowColorGradient(float s);
	void setHighColorGradient(float s);

	void SetLogFreq(int state);

private:
	GLuint m_vertexShader, m_fragmentShader;
	GLuint m_program;

	GLint positionLoc;

	GLint colorTextureLoc;
	GLint dataTextureLoc;

	GLint lowGradientLoc;
	GLint highGradientLoc;

	GLint logFreqLoc;

	const char *m_pVertexShader, *m_pFragmentShader;
};

#endif