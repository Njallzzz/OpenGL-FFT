#include "MusicController.h"
#include <iostream>

using std::cerr;
using std::endl;

MusicController::MusicController(int window_size, const char *settingsFile) : m_playbackDevice(OpenDevice()) {
	m_settingsFile = (char *) settingsFile;
	m_currentlySelected = -1;
	m_windowSize = window_size;
	m_logMagnitude = false;
	m_logFrequency = false;
	m_playbackBuffer = NULL;
	m_fftSamples = NULL;
	m_windowDataIn = NULL;
	m_magnitude = NULL;

	m_windowFunctionBuffer = new double[m_windowSize];
	GenerateHammingWindow();
	//GenerateSquareWindow();

	m_magnitude = new float[m_windowSize];
	m_windowDataIn = new double[m_windowSize];
	m_windowDataOut = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * m_windowSize);

	for (int i = 0; i < m_windowSize; i++) {
		m_magnitude[i] = 0;
	}

	plan = fftw_plan_dft_r2c_1d(m_windowSize, m_windowDataIn, m_windowDataOut, FFTW_ESTIMATE);

	if (LoadSettingsFile())
		return;
	if (m_settingsFileContent.size() > 0) {
		m_currentlySelected = 1;
		LoadMusicFile();
	}
}

MusicController::~MusicController() {
	delete[] m_windowFunctionBuffer;
	delete[] m_magnitude;
	delete[] m_windowDataIn;
	fftw_destroy_plan(plan);
	fftw_free(m_windowDataOut);

	if (m_fftSamples != NULL) {
		delete[] m_fftSamples;
	}
}

void
MusicController::TogglePlay() {
	if (m_playbackBuffer == NULL)
		return;

	if (m_playbackBuffer->isPlaying())
		m_playbackBuffer->stop();
	else
		m_playbackBuffer->play();
}

bool
MusicController::isPlaying() {
	if (m_playbackBuffer == NULL)
		return false;
	return m_playbackBuffer->isPlaying();
}

void
MusicController::SelectFile(int file) {
	if (m_settingsFileContent.find(file) == m_settingsFileContent.end()) {
		cerr << "Invalid file selection: " << file << endl;
		return;
	}
	m_currentlySelected = file;
	LoadMusicFile();
	m_playbackBuffer->play();
}

float *
MusicController::GetFFTBufferPtr() {
	return m_magnitude;
}

int
MusicController::GetSampleRate() {
	return m_sfInfo.samplerate;
}


float
MusicController::GetVolume() {
	return m_playbackBuffer->getVolume();
}

void
MusicController::SetVolume(float volume) {
	if (m_playbackBuffer == NULL)
		return;
	m_playbackBuffer->setVolume(volume);
}

int
MusicController::GetWindowSize() {
	return m_windowSize;
}

void
MusicController::SetWindowSize(int size) {
	m_windowSize = size;

	delete[] m_windowFunctionBuffer;
	delete[] m_magnitude;
	delete[] m_windowDataIn;
	fftw_free(m_windowDataOut);
	fftw_destroy_plan(plan);

	m_windowFunctionBuffer = new double[m_windowSize];
	GenerateHammingWindow();
	//GenerateSquareWindow();

	m_magnitude = new float[m_windowSize];
	m_windowDataIn = new double[m_windowSize];
	m_windowDataOut = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * m_windowSize);

	if (m_logMagnitude) {
		for (int i = 0; i < m_windowSize; i++) {
			m_magnitude[i] = -INFINITY;
		}
	} else {
		for (int i = 0; i < m_windowSize; i++) {
			m_magnitude[i] = 0;
		}
	}

	plan = fftw_plan_dft_r2c_1d(m_windowSize, m_windowDataIn, m_windowDataOut, FFTW_ESTIMATE);
}

float
MusicController::GetPitch() {
	return m_playbackBuffer->getPitchShift();
}

void MusicController::SetPitch(float pitch) {
	m_playbackBuffer->setPitchShift(pitch);
}

bool
MusicController::GetLogMagnitude() {
	return m_logMagnitude;
}

void
MusicController::SetLogMagnitude(bool state) {
	m_logMagnitude = state;
	if (!m_playbackBuffer->isPlaying()) {
		if (m_logMagnitude) {
			for (int i = 0; i < m_windowSize; i++) {
				m_magnitude[i] = -INFINITY;
			}
		} else {
			for (int i = 0; i < m_windowSize; i++) {
				m_magnitude[i] = 0;
			}
		}
	}
}

void
MusicController::Update() {
	if (!m_playbackBuffer->isPlaying())
		return;

	int offset = m_playbackBuffer->getPosition();
	if (offset + m_windowSize >= m_sfInfo.frames) {
		for (int i = 0; i < m_windowSize; i++) {
			m_magnitude[i] = 0;
		}
		return;
	}
	for (int i = 0; i < m_windowSize; i++) {
		m_windowDataIn[i] = m_fftSamples[offset + i] * m_windowFunctionBuffer[i];
	}

	fftw_execute(plan);

	int count = 1;
	for (int i = 64; i < m_windowSize; i = i * 2)
		count++;

	m_magnitude[0] = 0.0f;
	for (int i = 1; i < m_windowSize; i++) {
		m_magnitude[i] = (float) sqrt(pow(m_windowDataOut[i][0], 2.0) + pow(m_windowDataOut[i][1], 2.0));

		m_magnitude[i] *= (float) 6.0f / ((float)count * count);
	}
	if (m_logMagnitude) {
		for (int i = 0; i < m_windowSize; i++) {
			m_magnitude[i] = 20 * log(m_magnitude[i]);
		}
	}
}

bool
MusicController::LoadSettingsFile() {
	m_settingsFileContent.clear();

	ifstream file(m_settingsFile);
	if (!file.is_open()) {
		cerr << "Unable to open settings file: " << m_settingsFile << endl;
		return true;
	}

	int fileIndex = 1;
	string line;

	cout << "Settings file content:" << endl;
	while (getline(file, line)) {
		cout << fileIndex << ":\t" << line << endl;
		m_settingsFileContent[fileIndex++] = line;
	}

	file.close();
	return false;
}

bool
MusicController::LoadMusicFile() {
	if (m_currentlySelected == -1) {
		cerr << "Attempting to load an invalid file" << endl;
		return true;
	}
	SF_INFO sfInfo;
	SNDFILE* newFile = sf_open(m_settingsFileContent[m_currentlySelected].c_str(), SFM_READ, &sfInfo);
	if (newFile == NULL) {
		cerr << "Unable to load music file: " << m_settingsFileContent[m_currentlySelected] << endl;
		m_currentlySelected = -1;
		return true;
	}
	if (sfInfo.channels != 1) {
		cerr << "Attempting to load a music file which is not a mono: " << m_settingsFileContent[m_currentlySelected] << endl;
		m_currentlySelected = -1;
		return true;
	}

	m_sfInfo = sfInfo;

	cout << "Loaded file: " << m_settingsFileContent[m_currentlySelected] << endl;
	cout << "Audio channels: " << m_sfInfo.channels << endl;
	cout << "Sample rate: " << m_sfInfo.samplerate << endl;
	cout << "Frames: " << m_sfInfo.frames << endl;
	cout << "Format: " << m_sfInfo.format << endl;
	cout << "Sections: " << m_sfInfo.sections << endl;
	cout << "Seekable: " << m_sfInfo.seekable << endl;

	float volume = 1.0f;
	if (m_playbackBuffer != NULL)
		volume = m_playbackBuffer->getVolume();

	short *audiereSamples = new short[m_sfInfo.frames];
	sf_read_short(newFile, audiereSamples, m_sfInfo.frames);
	m_playbackBuffer = m_playbackDevice->openBuffer(audiereSamples, (int) m_sfInfo.frames, m_sfInfo.channels, m_sfInfo.samplerate, SF_S16);
	delete[] audiereSamples;

	if (m_fftSamples != NULL)
		delete[] m_fftSamples;
	m_fftSamples = new double[m_sfInfo.frames];
	sf_seek(newFile, 0, SEEK_SET);
	sf_read_double(newFile, m_fftSamples, m_sfInfo.frames);
	sf_close(newFile);

	m_playbackBuffer->setVolume(volume);
	return false;
}


void
MusicController::GenerateSquareWindow() {
	for (int i = 0; i < m_windowSize; i++) {
		m_windowFunctionBuffer[i] = 1.0;
	}
}

void
MusicController::GenerateHammingWindow() {
	for (int i = 0; i < m_windowSize; i++) {
		m_windowFunctionBuffer[i] =
			  0.54
			- 0.46 * cos(2 * M_PI * i / m_windowSize);
	}
}


void
MusicController::GenerateBlackmanHarrisWindow() {
	for (int i = 0; i < m_windowSize; i++) {
		m_windowFunctionBuffer[i] =
			  0.35875
			- 0.48829 * cos(2 * M_PI * i / (m_windowSize - 1))
			+ 0.14128 * cos(4 * M_PI * i / (m_windowSize - 1))
			- 0.01168 * cos(6 * M_PI * i / (m_windowSize - 1));
	}
}