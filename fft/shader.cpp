#include "shader.h"

Shader::Shader(	const char *vertexShaderFile,
				const char *fragmentShaderFile )
	: m_pVertexShader(vertexShaderFile)
	, m_pFragmentShader(fragmentShaderFile) {
	m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
	m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	m_program = glCreateProgram();
}


Shader::~Shader() {
	glDetachShader(m_program, m_vertexShader);
	glDetachShader(m_program, m_fragmentShader);

	glDeleteProgram(m_program);

	glDeleteShader(m_vertexShader);
	glDeleteShader(m_fragmentShader);
}

int
Shader::compile(ostream *stream) {
	ifstream vt(m_pVertexShader);
	ifstream ft(m_pFragmentShader);

	if(!(vt.is_open() && ft.is_open())) {
		*stream << "Unable to read souce shader files: \"" << m_pVertexShader << "\" and \"" << m_pFragmentShader << "\"" << endl;
		return 1;
	}

	string vertexShaderString;
	string fragmentShaderString;

	vt.seekg(0, std::ios::end);   
	vertexShaderString.reserve(vt.tellg());
	vt.seekg(0, std::ios::beg);

	ft.seekg(0, std::ios::end);   
	fragmentShaderString.reserve(ft.tellg());
	ft.seekg(0, std::ios::beg);

	vertexShaderString.assign((istreambuf_iterator<char>(vt)),
			istreambuf_iterator<char>());

	fragmentShaderString.assign((istreambuf_iterator<char>(ft)),
			istreambuf_iterator<char>());

	const GLchar *vertexShaderSource = (const GLchar *)vertexShaderString.c_str();
	const GLchar *fragmentShaderSource = (const GLchar *)fragmentShaderString.c_str();

	glShaderSource(m_vertexShader, 1, &vertexShaderSource, NULL);
	glShaderSource(m_fragmentShader, 1, &fragmentShaderSource, NULL);
	
	glCompileShader(m_vertexShader);
	glCompileShader(m_fragmentShader);

	GLint vertexShaderStatus = 0, fragmentShaderStatus = 0;
	glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &vertexShaderStatus);
	glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &fragmentShaderStatus);

	if(vertexShaderStatus == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(m_vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(m_vertexShader, maxLength, &maxLength, &errorLog[0]);
		*stream << "Vertex Compiler Output: " << endl << errorLog.data() << endl;
	}
	if(fragmentShaderStatus == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(m_fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(m_fragmentShader, maxLength, &maxLength, &errorLog[0]);
		*stream << "Fragment Compiler Output: " << endl << errorLog.data() << endl;
	}

	if(vertexShaderStatus == GL_FALSE || fragmentShaderStatus == GL_FALSE) {
		return 2;
	}

    glBindFragDataLocation(m_program, 0, "o_color");

	glAttachShader(m_program, m_vertexShader);
	glAttachShader(m_program, m_fragmentShader);

	glLinkProgram(m_program);

	GLint isLinked = 0;
	glGetProgramiv(m_program, GL_LINK_STATUS, (int *)&isLinked);
	if(isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(m_program, maxLength, &maxLength, &infoLog[0]);
		*stream << "Shader Linking Output: " << endl << infoLog.data() << endl;
		return 3;
	}

	positionLoc = glGetAttribLocation(m_program, "a_position");

	colorTextureLoc = glGetUniformLocation(m_program, "u_colorTexture");
	dataTextureLoc = glGetUniformLocation(m_program, "u_dataTexture");

	lowGradientLoc = glGetUniformLocation(m_program, "u_lowColorGradient");
	highGradientLoc = glGetUniformLocation(m_program, "u_highColorGradient");

	logFreqLoc = glGetUniformLocation(m_program, "u_logFreq");

	// Not needed until vao is defined
	//glEnableVertexAttribArray(positionLoc);

	*stream << "Compiled shader from: \"" << m_pVertexShader << "\" and \"" << m_pFragmentShader << "\"" << endl;
	return 0;
}

void
Shader::use() {
	glUseProgram(m_program);

	glUniform1i(colorTextureLoc, 0);
	glUniform1i(dataTextureLoc, 1);
}

GLint
Shader::getVertexPointer() {
	return positionLoc;
}

GLint
Shader::getColorTexturePointer() {
	return colorTextureLoc;
}

GLint
Shader::getDataTexturePointer() {
	return dataTextureLoc;
}

void
Shader::setLowColorGradient(float s) {
	glUniform1f(lowGradientLoc, s);
}

void
Shader::setHighColorGradient(float s) {
	glUniform1f(highGradientLoc, s);
}

void
Shader::SetLogFreq(int state) {
	glUniform1i(logFreqLoc, state);
}