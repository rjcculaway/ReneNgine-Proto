#include "rne_pipeline.hpp"
#include "rne_model.hpp"

#include <fstream>
#include <stdexcept>
#include <iostream>
#include <string>

namespace rne {
	RnePipeline::RnePipeline(
		RneDevice &device,
		const std::string &vertexFilePath,
		const std::string &fragmentFilePath,
		const PipelineConfigInfo& configInfo) : device{ device } {
		createGraphicsPipeline(vertexFilePath, fragmentFilePath, configInfo);
	}

	RnePipeline::~RnePipeline() {
		vkDestroyShaderModule(device.device(), vertexShaderModule, nullptr);
		vkDestroyShaderModule(device.device(), fragmentShaderModule, nullptr);
		vkDestroyPipeline(device.device(), graphicsPipeline, nullptr);
	}

	void RnePipeline::bind(VkCommandBuffer commandBuffer) {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	}

	PipelineConfigInfo RnePipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height) {
		PipelineConfigInfo configInfo{};

		// Input Assembly
		// Takes a list of vertices and groups them into geometry. In this case, triangles
		configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // Every three vertices are interpreted into triangles
		configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		// Viewport
		configInfo.viewport.x = 0.0f;
		configInfo.viewport.y = 0.0f;
		configInfo.viewport.width = static_cast<float>(width);
		configInfo.viewport.height = static_cast<float>(height);
		configInfo.viewport.minDepth = 0.0f;
		configInfo.viewport.maxDepth = 1.0f;

		// Scissor
		configInfo.scissor.offset = { 0, 0 };
		configInfo.scissor.extent = { width, height };

		// Rasterization
		// Breaks up geometry into fragments
		configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
		configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.rasterizationInfo.lineWidth = 1.0f;
		configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;
		configInfo.rasterizationInfo.depthBiasClamp = 0.0f;
		configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;

		// Multisample Anti-aliasing
		configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampleInfo.minSampleShading = 1.0f;
		configInfo.multisampleInfo.pSampleMask = nullptr;
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;

		// Color Blending
		// Controls how colors are combined
		configInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
		configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

		configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
		configInfo.colorBlendInfo.attachmentCount = 1;
		configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
		configInfo.colorBlendInfo.blendConstants[0] = 0.0f;
		configInfo.colorBlendInfo.blendConstants[1] = 0.0f;
		configInfo.colorBlendInfo.blendConstants[2] = 0.0f;
		configInfo.colorBlendInfo.blendConstants[3] = 0.0f;

		// Depth Testing
		configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depthStencilInfo.depthBoundsTestEnable = VK_TRUE;
		configInfo.depthStencilInfo.minDepthBounds = 0.0f;
		configInfo.depthStencilInfo.maxDepthBounds = 1.0f;
		configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.front = {};
		configInfo.depthStencilInfo.back = {};

		return configInfo;
	}

	std::vector<char> RnePipeline::readFile(const std::string& fileName) {
		std::ifstream fileHandle{ fileName, std::ios::ate | std::ios::binary };
		if (!fileHandle.is_open()) {
			throw std::runtime_error("Failed to open file" + fileName);
		}

		size_t fileSize = static_cast<size_t>(fileHandle.tellg());
		std::vector<char> buffer(fileSize);

		fileHandle.seekg(0);
		fileHandle.read(buffer.data(), fileSize);

		fileHandle.close();
		
		return buffer;
	}
	void RnePipeline::createGraphicsPipeline(
		const std::string& vertexFilePath, 
		const std::string& fragmentFilePath,
		const PipelineConfigInfo& configInfo) {
		std::vector<char> vertexCode = readFile(vertexFilePath);
		std::vector<char> fragmentCode = readFile(fragmentFilePath);

		//std::cout << "Vertex Code File Size " << vertexCode.size() << std::endl;
		//std::cout << "Fragment Code File Size " << fragmentCode.size() << std::endl;

		createShaderModule(vertexCode, &vertexShaderModule);
		createShaderModule(fragmentCode, &fragmentShaderModule);

		VkPipelineShaderStageCreateInfo shaderStages[2];
		// Vertex
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = vertexShaderModule;
		shaderStages[0].pName = "main";
		shaderStages[0].flags = 0;
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr;

		// Fragment
		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = fragmentShaderModule;
		shaderStages[1].pName = "main";
		shaderStages[1].flags = 0;
		shaderStages[1].pNext = nullptr;
		shaderStages[1].pSpecializationInfo = nullptr;

		// Determines how vertex buffer is interpreted
		auto bindingDescriptions = RneModel::Vertex::getBindingDescriptions();
		auto attributeDescriptions = RneModel::Vertex::getAttributeDescriptions();


		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

		VkPipelineViewportStateCreateInfo viewportInfo{};

		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = &configInfo.viewport;
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = &configInfo.scissor;

		// Pipeline
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;

		// Fill in the rest of the pipeline using the passed configInfo
		pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportInfo;
		pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
		pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
		pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
		pipelineInfo.pDynamicState = nullptr;

		pipelineInfo.layout = configInfo.pipelineLayout;
		pipelineInfo.renderPass = configInfo.renderPass;
		pipelineInfo.subpass = configInfo.subpass;

		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create graphics pipeline");
		}
	}

	void RnePipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
		VkShaderModuleCreateInfo createInfo{};

		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shader module");
		}
	}
}