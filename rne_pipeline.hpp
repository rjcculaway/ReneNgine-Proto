#pragma once

#include "rne_device.hpp"

#include <string>
#include <vector>

namespace rne {

	struct PipelineConfigInfo {
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class RnePipeline {
	public:
		RnePipeline(
			RneDevice &device, 
			const std::string& vertexFilePath, 
			const std::string& fragmentFilePath, 
			const PipelineConfigInfo &configInfo);
		~RnePipeline();

		RnePipeline(const RnePipeline&) = delete;
		void operator=(const RnePipeline&) = delete;

		static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
	private:
		static std::vector<char> readFile(const std::string& fileName);
		void createGraphicsPipeline(
			const std::string& vertexFilePath, 
			const std::string& fragmentFilePath, 
			const PipelineConfigInfo &configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		RneDevice& device;
		VkPipeline graphicsPipeline;
		VkShaderModule vertexShaderModule;
		VkShaderModule fragmentShaderModule;
	};
}