#pragma once

#include "rne_device.hpp"
#include "rne_swap_chain.hpp"
#include "rne_window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace rne {
    class RneRenderer {
    public:
        RneRenderer(RneWindow& window, RneDevice& device);
        ~RneRenderer();

        RneRenderer(const RneRenderer&) = delete;
        RneRenderer& operator=(const RneRenderer&) = delete;

        VkRenderPass getSwapChainRenderPass() const { return rneSwapChain->getRenderPass(); }
        float getAspectRatio() const { return rneSwapChain->extentAspectRatio(); };
        bool isFrameInProgress() const { return isFrameStarted; }

        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        RneWindow& rneWindow;
        RneDevice& rneDevice;
        std::unique_ptr<RneSwapChain> rneSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex;
        bool isFrameStarted;
    };
}  // namespace rne