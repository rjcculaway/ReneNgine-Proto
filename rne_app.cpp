#include "rne_app.hpp"
#include "rne_vertex_generator.hpp"

#include "keyboard_movement_controller.hpp"
#include "rne_camera.hpp"
#include "simple_render_system.hpp"

#include <stdexcept>
#include <array>
#include <chrono>

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
        // Flat Vase
        std::shared_ptr<RneModel> rneModel = RneModel::createModelFromFile(rneDevice, "models/flat_vase.obj");
        auto gameObject = RneGameObject::createGameObject();
        gameObject.model = rneModel;
        gameObject.transform.translation = {0.0f, 0.0f, 2.5f};
        gameObject.transform.scale = glm::vec3{ 3.f };
        
        gameObjects.push_back(std::move(gameObject));

        // Smooth Vase
        std::shared_ptr<RneModel> smoothVaseModel = RneModel::createModelFromFile(rneDevice, "models/smooth_vase.obj");
        auto smoothVase = RneGameObject::createGameObject();
        smoothVase.model = smoothVaseModel;
        smoothVase.transform.translation = { 2.0f, 0.0f, 2.5f };
        smoothVase.transform.scale = glm::vec3{ 3.f };

        gameObjects.push_back(std::move(smoothVase));
    }

	void RneApp::run() {
		SimpleRenderSystem simpleRenderSystem{ rneDevice, rneRenderer.getSwapChainRenderPass() };
        RneCamera camera{};
        //camera.setViewDirection(glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.5f, 0.0f, 1.0f });
        camera.setViewTarget(glm::vec3{ -1.0f, -2.0f, 2.0f }, glm::vec3{ 0.0f, 0.0f, 2.5f });
        
        auto viewerObject = RneGameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
        
        while (!rneWindow.shouldClose()) {
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(rneWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = rneRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.1f);

			if (auto commandBuffer = rneRenderer.beginFrame()) {
				rneRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
				rneRenderer.endSwapChainRenderPass(commandBuffer);
				rneRenderer.endFrame();
			}

			vkDeviceWaitIdle(rneDevice.device());
		}
	}
}