#pragma once

#include "rne_camera.hpp"

#include <vulkan/vulkan.h>

namespace rne {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		RneCamera &camera;
	};
}