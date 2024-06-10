#include "rne_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "rne_buffer.hpp"
#include "rne_camera.hpp"
#include "rne_frame_info.hpp"
#include "simple_render_system.hpp"

#include <stdexcept>
#include <array>
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace rne {

    struct GlobalUbo {
        alignas(16) glm::mat4 projectionView{ 1.0f };
        alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f));
    };

	RneApp::RneApp() {
        globalPool = RneDescriptorPool::Builder(rneDevice)
            .setMaxSets(RneSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, RneSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
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

        std::vector<std::unique_ptr<RneBuffer>> uboBuffers(RneSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<RneBuffer>(
                rneDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                rneDevice.properties.limits.minUniformBufferOffsetAlignment
            );
            uboBuffers[i]->map();
        }
        
        auto globalSetLayout = RneDescriptorSetLayout::Builder(rneDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
            .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(RneSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            RneDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

		SimpleRenderSystem simpleRenderSystem{ rneDevice, rneRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
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
                int frameIndex = rneRenderer.getFrameIndex();

                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex]
                };

                // prepare and update objects and memory
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();
    
                // render
				rneRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
				rneRenderer.endSwapChainRenderPass(commandBuffer);
				rneRenderer.endFrame();
			}

			vkDeviceWaitIdle(rneDevice.device());
		}
	}
}