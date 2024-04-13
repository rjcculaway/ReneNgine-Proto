#pragma once

#include "rne_app.hpp"
#include "rne_device.hpp"
#include "rne_pipeline.hpp"
#include "rne_swap_chain.hpp"
#include "rne_window.hpp"
#include "rne_model.hpp"
#include "rne_game_object.hpp"

#include <memory>
#include <vector>

namespace rne {
	class RneApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		RneApp();
		~RneApp();

		RneApp(const RneApp&) = delete;
		RneApp& operator=(const RneApp&) = delete;

		void run();
	private:
		void loadGameObjects();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
		void renderGameObjects(VkCommandBuffer commandBuffer);

		RneWindow rneWindow{ WIDTH, HEIGHT, "ReneNgine Proto" };
		RneDevice rneDevice{ rneWindow };
		std::unique_ptr<RneSwapChain> rneSwapChain;
		std::unique_ptr<RnePipeline> rnePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<RneGameObject> gameObjects;
		//RnePipeline rnePipeline { rneDevice, "./vertex.vert.spv", "./fragment.frag.spv", RnePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
	};
}