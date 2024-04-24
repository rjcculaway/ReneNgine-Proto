#pragma once

#include "rne_device.hpp"
#include "rne_pipeline.hpp"
#include "rne_game_object.hpp"

#include <memory>
#include <vector>

namespace rne {
	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(RneDevice& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<RneGameObject> &gameObjects);
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		RneDevice& rneDevice;

		std::unique_ptr<RnePipeline> rnePipeline;
		VkPipelineLayout pipelineLayout;
		//RnePipeline rnePipeline { rneDevice, "./vertex.vert.spv", "./fragment.frag.spv", RnePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
	};
}