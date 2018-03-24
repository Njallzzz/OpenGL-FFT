#include "window.h"
#include <cfloat>

Window::Window(ostream *stream, int width, int height,
	const char *windowName, int fps, int pseudodepth)
	: m_width(width), m_height(height), m_fps(fps)
	, m_pseudodepth(pseudodepth), m_pWindowName(windowName) {
	m_stream = stream;
	m_aspect = m_width / (float) m_height;
	m_pWindow = NULL;
	m_metrics = false;
	m_frameCount = 0;
	m_lastFrame = 0;
	m_worstFrame = 0.0;
	m_bestFrame = DBL_MAX;
}

Window::~Window() {
	if(m_pWindow != NULL) {
		glfwDestroyWindow(m_pWindow);
	}
	glfwTerminate();
}

int
Window::init() {
	int res = glfwInit();
	if(res == GLFW_FALSE) {
		*m_stream << "Fatal error: unable to initialize GLFW" << endl;
		return 1; 
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, 0);
	glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
	glfwWindowHint(GLFW_REFRESH_RATE, m_fps);

	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primary);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, m_pseudodepth);

	m_pWindow = glfwCreateWindow(m_width, m_height, m_pWindowName, NULL, NULL);
	if(m_pWindow == NULL) {
		*m_stream << "Fatal error: unable to create GLFW window" << endl;
		return 2; 
	}

	glfwMakeContextCurrent(m_pWindow);
	glfwSwapInterval(1);

	glfwGetFramebufferSize(m_pWindow, &m_width, &m_height);
	m_aspect = m_width / (float) m_height;

	*m_stream << "Opening window with resolution: " << m_width << "x" << m_height << "\t\t";
	*m_stream << "colordepth(R:" << mode->redBits << " G:" << mode->greenBits;
	*m_stream << " B:" << mode->blueBits << ")" << "\t\tfps: " << m_fps;
	*m_stream << "/" << mode->refreshRate << endl;
	return 0;
}

void 
Window::swapBuffers() {
	glfwSwapBuffers(m_pWindow);

	if(m_metrics) {
		m_frameCount++;
		double time = glfwGetTime();
		double frametime = time - m_lastFrame;
		if(time > 1.0) {
			*m_stream << "FPS: " << m_frameCount << "\t\t";
			*m_stream << "Worst: " << m_worstFrame * 1000.0 << "ms\t\t";
			*m_stream << "Best: " << m_bestFrame * 1000.0 << "ms";
			*m_stream << endl;
			m_frameCount = 0;
			m_worstFrame = 0.0;
			m_bestFrame = DBL_MAX;
			glfwSetTime(time - 1.0);
		}
		if(frametime < m_bestFrame) {
			m_bestFrame = frametime;
		}
		if(frametime > m_worstFrame) {
			m_worstFrame = frametime;
		}
		m_lastFrame = glfwGetTime();
	}
}

bool
Window::checkClosed() {
	return !glfwWindowShouldClose(m_pWindow);
}

bool
Window::MetricsStatus() {
	return m_metrics;
}

void
Window::showMetrics(bool status) {
	m_metrics = status;
	m_frameCount = 0;
	m_lastFrame = 0;
	m_worstFrame = 0.0;
	m_bestFrame = DBL_MAX;
	glfwSetTime(0.0);
}

float
Window::getAspect() {
	return m_aspect;
}

GLFWwindow*
Window::getWindowPointer() {
	return m_pWindow;
}