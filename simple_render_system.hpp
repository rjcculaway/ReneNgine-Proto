#pragma once

#include "rne_camera.hpp"
#include "rne_device.hpp"
#include "rne_pipeline.hpp"
#include "rne_game_object.hpp"
#include "rne_frame_info.hpp"

#include <memory>
#include <vector>

namespace rne {
	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(RneDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(FrameInfo& frameInfo);
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		RneDevice& rneDevice;

		std::unique_ptr<RnePipeline> rnePipeline;
		VkPipelineLayout pipelineLayout;
		//RnePipeline rnePipeline { rneDevice, "./vertex.vert.spv", "./fragment.frag.spv", RnePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
	};
}