#include "Texture1D.h"
#include <iostream>

using std::cerr;
using std::endl;

Texture1D::Texture1D(Texture1DSource source, const unsigned char* buffer, int width = 0) {
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_1D, m_texture);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (source == Texture1DSource::BUFFER) {
		if (width <= 0) {
			cerr << "Texture1D attempting to load buffer but buffer of size: " << width << " was provided" << endl;
			return;
		}
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, width, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	} else if (source == Texture1DSource::FILE) {
		// Not implemented
	}
}

Texture1D::~Texture1D() {
	glDeleteTextures(1, &m_texture);
}

void
Texture1D::Bind(GLint unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_1D, m_texture);
}
