#include "rne_app.hpp"

namespace rne {
	void RneApp::run() {
		while (!rneWindow.shouldClose()) {
			glfwPollEvents();
		}
	}
}