#include "rne_model.hpp"
#include "rne_utils.hpp"

// Libraries
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <cstring>
#include <iostream>
#include <unordered_map>

namespace std {
	template<>
	struct hash<rne::RneModel::Vertex> {
		size_t operator()(rne::RneModel::Vertex const& vertex) const {
			size_t seed = 0;
			rne::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace rne {
	RneModel::RneModel(RneDevice& device, const RneModel::Builder& builder) : rneDevice{ device } {
		createVertexBuffers(builder.vertices);
		createIndexBuffers(builder.indices);
	}
	RneModel::~RneModel() {}

	std::unique_ptr<RneModel> RneModel::createModelFromFile(RneDevice &device, const std::string& filePath) {
		Builder builder{};
		builder.loadModel(filePath);

		std::cout << "Number of vertices: " << builder.vertices.size() << "\n";

		return std::make_unique<RneModel>(device, builder);
	}

	void RneModel::draw(VkCommandBuffer commandBuffer) {
		if (hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
		}
	}
	void RneModel::bind(VkCommandBuffer commandBuffer) {
		VkBuffer buffers[] = { vertexBuffer->getBuffer()};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void RneModel::createVertexBuffers(const std::vector<Vertex>& vertices) {
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3.");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

		uint32_t vertexSize = sizeof(vertices[0]);

		// Staging buffer to copy to device local memory
		RneBuffer stagingBuffer{
			rneDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());

		vertexBuffer = std::make_unique<RneBuffer>(
			rneDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		// Transfer staging buffer to device local vertex buffer
		rneDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
	}

	void RneModel::createIndexBuffers(const std::vector<uint32_t>& indices) {
		indexCount = static_cast<uint32_t>(indices.size());
		uint32_t indexSize = sizeof(indices[0]);
		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
		
		hasIndexBuffer = indexCount > 0;
		if (!hasIndexBuffer) 
			return;
		
		// Staging buffer to copy to device local memory
		RneBuffer stagingBuffer{
			rneDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());

		indexBuffer = std::make_unique<RneBuffer>(
			rneDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		// Transfer staging buffer to device local index buffer
		rneDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
	}

	std::vector<VkVertexInputBindingDescription> RneModel::Vertex::getBindingDescriptions() {
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> RneModel::Vertex::getAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(4);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);
		
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, normal);

		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex, uv);

		return attributeDescriptions;
	}

	void RneModel::Builder::loadModel(const std::string& filePath) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{}; // Keep track of unique vertices so that the index buffer can instead be utilized for repeating vertices
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				// Vertex values loaded are tightly packed in their corresponding arrays, so we need a stride and offset to acquire the values
				if (index.vertex_index >= 0) {
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};

					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2]
					};
				}

				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}

				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}

				if (uniqueVertices.count(vertex) == 0) {
					// We use vertices.size() since this will give the current index of the vertex
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}

		}
		
		return;
	}
}