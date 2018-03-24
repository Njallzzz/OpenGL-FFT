#include "FFTWindow.h"

FFTWindow::FFTWindow(int windowSize, const char *musicLookupFile) :
	m_controller(windowSize, musicLookupFile),
	m_shader("Assets/graph.vert", "Assets/graph.frag")
{
	bottom = -40;
	top = 110;

	m_logFreq = true;
	m_logMagn = false;

	m_shader.compile(&std::cout);
	m_shader.use();
	m_shader.SetLogFreq(1);

	cout << "Color Texture Location: " << m_shader.getColorTexturePointer() << endl;
	cout << "Data Texture Location:  " << m_shader.getDataTexturePointer() << endl;

	const unsigned char m_colorBuffer[] = {
		0x00, 0x00, 0xFF,	// Blue
		0x00, 0xFF, 0xFF,	// Cyan
		0x00, 0xFF, 0x00,	// Green
		0xFF, 0xFF, 0x00,	// Yellow
		0xFF, 0xA5, 0x00,	// Orange
		0xFF, 0x00, 0x00,	// Red
	};
	m_colorTexture = new Texture1D(Texture1D::Texture1DSource::BUFFER, m_colorBuffer, sizeof(m_colorBuffer) / 3);

	m_shader.setHighColorGradient(1.0);
	m_shader.setLowColorGradient(0.0);

	m_outputBufferSize = m_controller.GetWindowSize();
	m_outputBuffer = m_controller.GetFFTBufferPtr();

	ResizeOutputBuffer();

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	const float vertices[] = {
		0.0f, 0.0f,		// Left Bottom Corner
		1.0f, 0.0f,		// Right Bottom Corner
		0.0f, 1.0f,		// Left Top Corner
		1.0f, 1.0f		// Right Top Corner
	};
	glGenBuffers(1, &m_baseMesh);
	glBindBuffer(GL_ARRAY_BUFFER, m_baseMesh);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(m_shader.getVertexPointer());
	glVertexAttribPointer(m_shader.getVertexPointer(), 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

FFTWindow::~FFTWindow() {
	delete[] m_dataBuffer;
	delete m_colorTexture;
	glDeleteBuffers(1, &m_baseMesh);
	glDeleteVertexArrays(1, &m_vao);
}

void
FFTWindow::Draw() {
	m_controller.Update();
	m_shader.use();

	glBindVertexArray(m_vao);

	if (m_logMagn) {
		for (int i = 0; i < m_dataBufferSize; i++) {
			if (m_outputBuffer[i] == -INFINITY)
				m_outputBuffer[i] = bottom;
			float res = ((m_outputBuffer[i] - bottom) / (top - bottom));
			if (res < 0)
				res = 0;
			else if (res > 1.0)
				res = 1.0;
			m_dataBuffer[i] = res * 255;
		}
	}
	else {
		for (int i = 0; i < m_dataBufferSize; i++) {
			float res = m_outputBuffer[i] / 60.0f;
			if (res < 0)
				res = 0;
			else if (res > 1.0)
				res = 1.0;
			m_dataBuffer[i] = res * 255;
		}
	}
	Texture1D dataRange(Texture1D::Texture1DSource::BUFFER, m_dataBuffer, (m_dataBufferSize / 3) + 1);

	m_colorTexture->Bind(0);
	dataRange.Bind(1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void
FFTWindow::TogglePlay() {
	m_controller.TogglePlay();
}

void
FFTWindow::SelectFile(int file) {
	m_controller.SelectFile(file);

	delete[] m_dataBuffer;
	ResizeOutputBuffer();
}

float
FFTWindow::GetPitch() {
	return m_controller.GetPitch();
}

void
FFTWindow::SetPitch(float pitch) {
	m_controller.SetPitch(pitch);
}

bool
FFTWindow::GetLogMagnitude() {
	return m_controller.GetLogMagnitude();
}

void
FFTWindow::SetLogMagnitude(bool state) {
	m_controller.SetLogMagnitude(state);
	m_logMagn = state;
}

bool
FFTWindow::GetLogFrequency() {
	return m_logFreq;
}

void
FFTWindow::SetLogFrequency(bool state) {
	if (state)
		m_shader.SetLogFreq(1);
	else
		m_shader.SetLogFreq(0);
	m_logFreq = state;
}

int
FFTWindow::GetWindowSize() {
	return m_outputBufferSize;
}

void
FFTWindow::SetWindowSize(int size) {
	m_controller.SetWindowSize(size);
	m_outputBufferSize = m_controller.GetWindowSize();
	m_outputBuffer = m_controller.GetFFTBufferPtr();

	delete[] m_dataBuffer;
	ResizeOutputBuffer();
}

void
FFTWindow::ResizeOutputBuffer() {
	int sampleRate = m_controller.GetSampleRate();
	m_dataBufferSize = (m_outputBufferSize / 2) * 20000 / sampleRate;
	m_dataBuffer = new unsigned char[m_dataBufferSize];
}