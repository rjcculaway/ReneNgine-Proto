#pragma once

#include "rne_device.hpp"

#include <string>
#include <vector>

namespace rne {

	struct PipelineConfigInfo {
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
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
		RnePipeline& operator=(const RnePipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);
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