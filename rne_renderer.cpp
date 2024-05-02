#include "rne_renderer.hpp"

#include <stdexcept>
#include <array>


namespace rne {

	RneRenderer::RneRenderer(RneWindow& window, RneDevice& device) : rneWindow{ window }, rneDevice{device} {
		recreateSwapChain();
		createCommandBuffers();
	}

	RneRenderer::~RneRenderer() {
		freeCommandBuffers();
	}


	void RneRenderer::recreateSwapChain() {
		auto extent = rneWindow.getExtent();

		while (extent.width == 0 || extent.height == 0) {
			extent = rneWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(rneDevice.device());
		if (rneSwapChain == nullptr) {
			rneSwapChain = std::make_unique<RneSwapChain>(rneDevice, extent);
		}
		else {
			std::shared_ptr<RneSwapChain> oldSwapChain = std::move(rneSwapChain);
			rneSwapChain = std::make_unique<RneSwapChain>(rneDevice, extent, oldSwapChain);
			
			if (!oldSwapChain->compareSwapFormats(*rneSwapChain.get())) {
				throw std::runtime_error("Swap chain image (or depth) format has changed!");
			}
		}
	}

	void RneRenderer::createCommandBuffers() {
		commandBuffers.resize(RneSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = rneDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(rneDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void RneRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(rneDevice.device(), rneDevice.getCommandPool(), static_cast<float>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer RneRenderer::beginFrame() {
		assert(!isFrameStarted && "Cannot call beginFrame with frame already in progress");

		auto result = rneSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		isFrameStarted = true;
		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		return commandBuffer;
	}

	void RneRenderer::endFrame() {
		assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = rneSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
			rneWindow.wasWindowResized()) {
			rneWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % RneSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void RneRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't begin swap chain render pass without frame in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer on different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = rneSwapChain->getRenderPass();
		renderPassInfo.framebuffer = rneSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = rneSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(rneSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(rneSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, rneSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void RneRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't end swap chain render pass without frame in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer on different frame");
	
		vkCmdEndRenderPass(commandBuffer);
	}
}