#include "rne_window.hpp"

#include <stdexcept>

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
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetWindowSizeCallback(window, framebufferResizeCallback);
	}

	void RneWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface");
		}
	}

	void RneWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto rneWindow = reinterpret_cast<RneWindow*>(glfwGetWindowUserPointer(window));
		rneWindow->framebufferResized = true;
		rneWindow->width = width;
		rneWindow->height = height;
	}
}