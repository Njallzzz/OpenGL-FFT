#pragma once
#define _USE_MATH_DEFINES
#include <iostream>
#include <fftw3.h>
#include <sndfile.h>
#include <audiere.h>
#include <map>
#include <string>
#include <fstream>

using namespace std;
using namespace audiere;

class MusicController {
public:
	MusicController(int window_size, const char *settingsFile);
	~MusicController();

	void TogglePlay();
	bool isPlaying();

	void SelectFile(int file);

	float *GetFFTBufferPtr();

	int GetSampleRate();

	float GetVolume();
	void SetVolume(float volume);

	int GetWindowSize();
	void SetWindowSize(int size);

	float GetPitch();
	void SetPitch(float pitch);

	bool GetLogMagnitude();
	void SetLogMagnitude(bool state);

	void Update();

private:
	int m_ampSteps;

	bool LoadSettingsFile();
	bool LoadMusicFile();
	void GenerateSquareWindow();
	void GenerateHammingWindow();
	void GenerateBlackmanHarrisWindow();

	char *m_settingsFile;
	map<int, string> m_settingsFileContent;
	int m_currentlySelected;

	bool m_logMagnitude, m_logFrequency;

	AudioDevicePtr m_playbackDevice;
	OutputStreamPtr m_playbackBuffer;

	SF_INFO m_sfInfo;

	double *m_fftSamples;

	fftw_plan plan;
	int m_windowSize;
	double *m_windowDataIn;
	float *m_magnitude;
	fftw_complex *m_windowDataOut;
	double *m_windowFunctionBuffer;
};

