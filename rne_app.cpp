#include "rne_app.hpp"
#include "rne_vertex_generator.hpp"
#include "simple_render_system.hpp"

#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace rne {

	RneApp::RneApp() {
		loadGameObjects();
	}

	RneApp::~RneApp() {
	}

	void RneApp::loadGameObjects() {
		//std::vector<RneModel::Vertex> vertices{
		//	{{0.0f, -0.5f}},
		//	{{0.5f, 0.5f}},
		//	{{-0.5f, 0.5f}}
		//};

		auto rneModel = std::make_shared<RneModel>(rneDevice, RneVertexGenerator::triangle());
		auto triangle = RneGameObject::createGameObject();
		triangle.model = rneModel;
		triangle.color = { 0.1f, 0.8f, 0.1f };
		triangle.transform2d.translation.x = 0.2f;
		triangle.transform2d.scale = { 2.0f, 0.5f };
		triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

		gameObjects.push_back(std::move(triangle));
	}

	void RneApp::run() {
		SimpleRenderSystem simpleRenderSystem{ rneDevice, rneRenderer.getSwapChainRenderPass() };
		while (!rneWindow.shouldClose()) {
			glfwPollEvents();
			
			if (auto commandBuffer = rneRenderer.beginFrame()) {
				rneRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
				rneRenderer.endSwapChainRenderPass(commandBuffer);
				rneRenderer.endFrame();
			}

			vkDeviceWaitIdle(rneDevice.device());
		}
	}
}