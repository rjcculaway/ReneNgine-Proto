#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <string>

namespace rne {

	class RneWindow {
	public:
		RneWindow(int w, int h, std::string name);
		~RneWindow();

		RneWindow(const RneWindow&) = delete;
		RneWindow &operator=(const RneWindow&) = delete;
		bool wasWindowResized() { return framebufferResized; };
		void resetWindowResizedFlag() { framebufferResized = false; };

		GLFWwindow* getGLFWwindow() const { return window; }

		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; };

		void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
	private:
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();

		int width;
		int height;
		bool framebufferResized = false;

		std::string windowName;

		GLFWwindow* window;
	};
}