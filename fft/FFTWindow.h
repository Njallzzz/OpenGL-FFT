#pragma once
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "MusicController.h"
#include "shader.h"
#include "Texture1D.h"

using namespace std;

class FFTWindow {
public:
	FFTWindow(int windowSize, const char *musicLookupFile);
	~FFTWindow();

	void Draw();

	// Music Related
	void TogglePlay();
	void SelectFile(int file);

	float GetPitch();
	void SetPitch(float pitch);

	bool GetLogMagnitude();
	void SetLogMagnitude(bool state);

	bool GetLogFrequency();
	void SetLogFrequency(bool state);

	int GetWindowSize();
	void SetWindowSize(int size);

private:
	void ResizeOutputBuffer();

	MusicController m_controller;
	Shader m_shader;

	float bottom, top;
	bool m_logFreq, m_logMagn;

	int m_outputBufferSize;
	float *m_outputBuffer;

	int m_dataBufferSize;
	unsigned char *m_dataBuffer;

	Texture1D *m_colorTexture;
	Texture1D *m_dataTexture;

	GLuint m_vao, m_baseMesh;
};

