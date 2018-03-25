#include "FFTWindow.h"

FFTWindow::FFTWindow(int windowSize, const char *musicLookupFile) :
	m_controller(windowSize, musicLookupFile),
	m_shader("Assets/graph.vert", "Assets/graph.frag")
{
	bottom = -60;
	top = 93;

	m_logFreq = false;
	m_logMagn = false;
	m_grid = true;

	m_shader.Compile(&std::cout);
	m_shader.Use();
	m_shader.SetLogFreq(0);

	cout << "Color Texture Location: " << m_shader.GetColorTexturePointer() << endl;
	cout << "SSBO Location:          " << m_shader.GetDataPointer() << endl;

	const unsigned char m_colorBuffer[] = {
		0x00, 0x00, 0xFF,	// Blue
		0x00, 0xFF, 0xFF,	// Cyan
		0x00, 0xFF, 0x00,	// Green
		0xFF, 0xFF, 0x00,	// Yellow
		0xFF, 0xA5, 0x00,	// Orange
		0xFF, 0x00, 0x00,	// Red
	};
	m_colorTexture = new Texture1D(Texture1D::Texture1DSource::BUFFER, m_colorBuffer, sizeof(m_colorBuffer) / 3);

	m_shader.SetHighColorGradient(1.0);
	m_shader.SetLowColorGradient(0.0);

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

	glEnableVertexAttribArray(m_shader.GetVertexPointer());
	glVertexAttribPointer(m_shader.GetVertexPointer(), 2, GL_FLOAT, GL_FALSE, 0, NULL);

	float gridVertex[120];
	for (int i = 0; i < 20; i++) {					// Vertical Lines
		gridVertex[i * 4] = i / 20.0f;		// Bottom
		gridVertex[(i * 4) + 1] = 0;

		gridVertex[(i * 4) + 2] = i / 20.0f;	// Top
		gridVertex[(i * 4) + 3] = 1;
	}
	for (int i = 0; i < 10; i++) {					// Horizonal Lines
		gridVertex[(i * 4) + 80] = 0;				// Left
		gridVertex[(i * 4) + 81] = i / 10.0f;

		gridVertex[(i * 4) + 82] = 1;				// Right
		gridVertex[(i * 4) + 83] = i / 10.0f;
	}

	glGenBuffers(1, &m_gridMesh);
	glBindBuffer(GL_ARRAY_BUFFER, m_gridMesh);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gridVertex), gridVertex, GL_STATIC_DRAW);

	float logGridVertex[72];
	for (int i = 0; i < 9; i++) {
		logGridVertex[i * 4] = ((i + 1) / 10.0f) * 0.005f;
		logGridVertex[(i * 4) + 1] = 0;

		logGridVertex[(i * 4) + 2] = logGridVertex[i * 4];
		logGridVertex[(i * 4) + 3] = 1;
	}
	for (int i = 0; i < 9; i++) {
		logGridVertex[(i * 4) + 36] = ((i + 1) / 10.0f) * 0.05f;
		logGridVertex[(i * 4) + 37] = 0;

		logGridVertex[(i * 4) + 38] = logGridVertex[(i * 4) + 36];
		logGridVertex[(i * 4) + 39] = 1;
	}

	glGenBuffers(1, &m_logGridMesh);
	glBindBuffer(GL_ARRAY_BUFFER, m_logGridMesh);
	glBufferData(GL_ARRAY_BUFFER, sizeof(logGridVertex), logGridVertex, GL_STATIC_DRAW);

	glGenBuffers(1, &m_databufferObject);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_databufferObject);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_dataBufferSize * sizeof(float), NULL, GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_databufferObject);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_databufferObject);
}

FFTWindow::~FFTWindow() {
	delete m_colorTexture;
	glDeleteBuffers(1, &m_databufferObject);
	glDeleteBuffers(1, &m_baseMesh);
	glDeleteVertexArrays(1, &m_vao);
}

void
FFTWindow::Draw() {
	m_controller.Update();
	m_shader.Use();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_databufferObject);
	float* p = (float*) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	if (m_logMagn) {
		for (int i = 0; i < m_dataBufferSize; i++) {
			if (m_outputBuffer[i] == -INFINITY)
				m_outputBuffer[i] = bottom;
			float res = ((m_outputBuffer[i] - bottom) / (top - bottom));
			if (res < 0)
				res = 0;
			else if (res > 1.0)
				res = 1.0;
			p[i] = res;
		}
	} else {
		for (int i = 0; i < m_dataBufferSize; i++) {
			float res = m_outputBuffer[i] / 10.0f;
			if (res > 0.5f) {
				float original = res;

				res = 20 * log(m_outputBuffer[i]);
				res = ((res - bottom) / (top - bottom));
			}
			if (res < 0)
				res = 0;
			else if (res > 1.0)
				res = 1.0;
			p[i] = res;
		}
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(m_vao);

	m_shader.SetGridLines(0.0f, 0.0f, 0.0f, 0.0f);
	glBindBuffer(GL_ARRAY_BUFFER, m_baseMesh);
	glVertexAttribPointer(m_shader.GetVertexPointer(), 2, GL_FLOAT, GL_FALSE, 0, NULL);
	m_colorTexture->Bind(0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	if (m_grid) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_shader.SetGridLines(1.0f, 1.0f, 1.0f, 0.2f);
		glBindBuffer(GL_ARRAY_BUFFER, m_gridMesh);
		glVertexAttribPointer(m_shader.GetVertexPointer(), 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glDrawArrays(GL_LINES, 0, 60);

		if (m_logFreq) {
			//m_shader.SetGridLines(1.0f, 1.0f, 1.0f, 0.2f);
			glBindBuffer(GL_ARRAY_BUFFER, m_logGridMesh);
			glVertexAttribPointer(m_shader.GetVertexPointer(), 2, GL_FLOAT, GL_FALSE, 0, NULL);
			glDrawArrays(GL_LINES, 0, 36);
		}

		glDisable(GL_BLEND);
	}

	glEnable(GL_DEPTH_TEST);
}

void
FFTWindow::TogglePlay() {
	m_controller.TogglePlay();
}

void
FFTWindow::SelectFile(int file) {
	m_controller.SelectFile(file);

	ResizeOutputBuffer();
}

float
FFTWindow::GetVolume() {
	return m_controller.GetVolume();
}

void
FFTWindow::SetVolume(float volume) {
	m_controller.SetVolume(volume);
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

	ResizeOutputBuffer();
}

bool
FFTWindow::GetGrid() {
	return m_grid;
}

void
FFTWindow::SetGrid(bool state) {
	m_grid = state;
}

void
FFTWindow::ResizeOutputBuffer() {
	int sampleRate = m_controller.GetSampleRate();
	m_dataBufferSize = (m_outputBufferSize / 2) * 20000 / (sampleRate / 2);
	m_shader.SetDataSize(m_dataBufferSize);

	glDeleteBuffers(1, &m_databufferObject);
	glGenBuffers(1, &m_databufferObject);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_databufferObject);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_dataBufferSize * sizeof(float), NULL, GL_STREAM_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_databufferObject);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_databufferObject);
}