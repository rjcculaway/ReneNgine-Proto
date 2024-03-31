#include "rne_app.hpp"
#include "rne_vertex_generator.hpp"

#include <stdexcept>
#include <array>

namespace rne {

	RneApp::RneApp() {
		loadModels();
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}

	RneApp::~RneApp() {
		vkDestroyPipelineLayout(rneDevice.device(), pipelineLayout, nullptr);
	}

	void RneApp::loadModels() {
		//std::vector<RneModel::Vertex> vertices{
		//	{{0.0f, -0.5f}},
		//	{{0.5f, 0.5f}},
		//	{{-0.5f, 0.5f}}
		//};

		rneModel = std::make_unique<RneModel>(rneDevice, RneVertexGenerator::siepernski_triangle(7));
	}

	void RneApp::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(rneDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}
	void RneApp::createPipeline() {
		auto pipelineConfig = RnePipeline::defaultPipelineConfigInfo(rneSwapChain.width(), rneSwapChain.height());
		pipelineConfig.renderPass = rneSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		rnePipeline = std::make_unique<RnePipeline>(
			rneDevice, 
			"./vertex.vert.spv", 
			"./fragment.frag.spv", 
			pipelineConfig);
	}
	void RneApp::createCommandBuffers() {
		commandBuffers.resize(rneSwapChain.imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = rneDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(rneDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}

		for (int i = 0; i < commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("Failed to begin recording command buffer");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = rneSwapChain.getRenderPass();
			renderPassInfo.framebuffer = rneSwapChain.getFrameBuffer(i);

			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = rneSwapChain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };

			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			rnePipeline->bind(commandBuffers[i]);
			rneModel->bind(commandBuffers[i]);
			rneModel->draw(commandBuffers[i]);

			vkCmdEndRenderPass(commandBuffers[i]);

			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to end recording command buffer");
			}
		}
	}
	void RneApp::drawFrame() {
		uint32_t imageIndex;

		auto result = rneSwapChain.acquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		result = rneSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to submit command buffer");
		}
	}

	void RneApp::run() {
		while (!rneWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();

			vkDeviceWaitIdle(rneDevice.device());
		}
	}
}