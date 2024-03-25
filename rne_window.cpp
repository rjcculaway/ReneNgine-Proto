#include "rne_window.hpp"

namespace rne {
	RneWindow::RneWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
		initWindow();
	}

	RneWindow::~RneWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void RneWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	}
}