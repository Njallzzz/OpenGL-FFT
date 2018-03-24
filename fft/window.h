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

	int init();

	void swapBuffers();
	bool checkClosed();

	bool MetricsStatus();
	void showMetrics(bool status);

	float getAspect();
	GLFWwindow* getWindowPointer();
private:
	int m_width, m_height, m_fps, m_pseudodepth;
	float m_aspect;
	double m_lastFrame, m_worstFrame, m_bestFrame;
	bool m_metrics;
	int m_frameCount;
	const char *m_pWindowName;
	GLFWwindow *m_pWindow;
	ostream *m_stream;
};

#endif // _WINDOW_H_