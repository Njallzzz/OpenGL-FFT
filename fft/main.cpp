#define _USE_MATH_DEFINES
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <Windows.h>
#include <fftw3.h>
#include <sndfile.h>
#include <audiere.h>
#include <cmath>

using namespace std;

#include "window.h"
#include "FFTWindow.h"

Window *window;
FFTWindow *fft;

void error_callback(int error, const char* description) {
	puts(description);
}

static bool wireframe = false;

void key_callback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
	if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9 && action == GLFW_PRESS) {
		short button = key - 48;
		fft->SelectFile(button);
		return;
	} else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) { return; }

	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		window->ShowMetrics(!window->MetricsStatus());
	}
	else if (key == GLFW_KEY_M) { }
	else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		window->SetFullScreen(!window->GetFullscreen());
	}
	else if (key == GLFW_KEY_F) {}
	else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		if (wireframe) {
			cout << "Raserization mode changed to normal" << endl;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			wireframe = false;
		}
		else {
			cout << "Raserization mode changed to wireframe" << endl;
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			wireframe = true;
		}
	}
	else if (key == GLFW_KEY_W) { }
	else if (key == GLFW_KEY_G && action == GLFW_PRESS) {
		bool grid = fft->GetGrid();
		fft->SetGrid(!grid);
		if (grid) {
			cout << "Turning on grid" << endl;
		} else {
			cout << "Turning off grid" << endl;
		}
	}
	else if (key == GLFW_KEY_G) {}
	else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		int peeksize = fft->GetWindowSize();
		if (peeksize <= 64)
			return;
		peeksize /= 2;
		fft->SetWindowSize(peeksize);
		cout << "Lowering window size to: " << peeksize << endl;
	} else if (key == GLFW_KEY_A) { }
	else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
		int peeksize = fft->GetWindowSize();
		if (peeksize >= (1 << 16))
			return;
		peeksize *= 2;
		fft->SetWindowSize(peeksize);
		cout << "Raising window size to: " << peeksize << endl;
	}
	else if (key == GLFW_KEY_Q) {}
	else if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		float peekpitch = fft->GetPitch();
		peekpitch -= 0.01f;
		if (peekpitch < 0.5f)
			peekpitch = 0.5f;
		fft->SetPitch(peekpitch);
		cout << "Setting pitch to: " << peekpitch << endl;
	}
	else if (key == GLFW_KEY_LEFT) {}
	else if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		float peekpitch = fft->GetPitch();
		peekpitch += 0.01f;
		if (peekpitch > 2.0f)
			peekpitch = 2.0f;
		fft->SetPitch(peekpitch);
		cout << "Setting pitch to: " << peekpitch << endl;
	}
	else if (key == GLFW_KEY_RIGHT) {}
	else if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		float peekVolume = fft->GetVolume();
		peekVolume += 0.01f;
		if (peekVolume > 1.0f)
			peekVolume = 1.0f;
		fft->SetVolume(peekVolume);
		cout << "Setting volume to: " << peekVolume << endl;
	}
	else if (key == GLFW_KEY_UP) {}
	else if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		float peekVolume = fft->GetVolume();
		peekVolume -= 0.01f;
		if (peekVolume < 0.0f)
			peekVolume = 0.0f;
		fft->SetVolume(peekVolume);
		cout << "Setting volume to: " << peekVolume << endl;
	}
	else if (key == GLFW_KEY_DOWN) {}
	else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		fft->TogglePlay();
	} else if (key == GLFW_KEY_SPACE) { }
	else if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		bool res = fft->GetLogFrequency();
		fft->SetLogFrequency(!res);
		if (res)
			cout << "Setting frequency spacing to linear" << endl;
		else
			cout << "Setting frequency spacing to logarthimic" << endl;
	}
	else if (key == GLFW_KEY_X) {}
	else if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
		bool res = fft->GetLogMagnitude();
		fft->SetLogMagnitude(!res);
		if (res)
			cout << "Setting amplitude to linear" << endl;
		else
			cout << "Setting amplitude to logarthimic" << endl;
	}
	else if (key == GLFW_KEY_Y) {}
	else {
		cout << "Key: " << key << " Scancode: " << scancode << " Action: " << action << " mods: " << mods << endl;
	}
}

int main(int argc, char **argv) {
	wireframe = false;
	glfwSetErrorCallback(error_callback);
	int width = 1536, height = 864;
	window = new Window(&std::cout, width, height, "Fast Fourier Transform Window", 120, 8);
	int res = window->Init();
	if (res) {
		return res;
	}
	res = gl3wInit();
	if (res) {
		std::cout << "Fatal error, unable to initalize gl3w" << std::endl;
		return res;
	}

	GLFWwindow *pWindow = window->GetWindowPointer();

	fft = new FFTWindow(4096, "files.txt");
	glfwSetKeyCallback(pWindow, key_callback);

	glEnable(GL_DEPTH_TEST);
	while (window->CheckClosed()) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		fft->Draw();

		window->SwapBuffers();
		glfwPollEvents();
	}
	return 0;
}
