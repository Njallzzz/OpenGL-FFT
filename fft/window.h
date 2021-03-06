#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <GLFW/glfw3.h>
#include <iostream>

using std::ostream;
using std::endl;

class Window {
public:
	Window(ostream *stream, int width, int height,
		const char *windowName, int fps, int pseudodepth);
	~Window();

	int Init();

	void Update();

	void SwapBuffers();
	bool CheckClosed();

	bool MetricsStatus();
	void ShowMetrics(bool status);

	float GetAspect();
	GLFWwindow* GetWindowPointer();

	bool GetFullscreen();
	void SetFullScreen(bool fullscreen);


private:

	int m_width, m_height, m_fps, m_pseudodepth;
	float m_aspect;
	double m_lastFrame, m_worstFrame, m_bestFrame;
	bool m_metrics;
	int m_frameCount;
	int m_pos[2];
	const char *m_pWindowName;
	GLFWwindow *m_pWindow;
	ostream *m_stream;
};

#endif // _WINDOW_H_