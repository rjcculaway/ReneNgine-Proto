#include "rne_pipeline.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>


namespace rne {
	RnePipeline::RnePipeline(const std::string& vertexFilePath, const std::string& fragmentFilePath) {
		createGraphicsPipeline(vertexFilePath, fragmentFilePath);
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
	void RnePipeline::createGraphicsPipeline(const std::string& vertexFilePath, const std::string& fragmentFilePath) {
		std::vector<char> vertexCode = readFile(vertexFilePath);
		std::vector<char> fragmentCode = readFile(fragmentFilePath);

		std::cout << "Vertex Code File Size " << vertexCode.size() << std::endl;
		std::cout << "Fragment Code File Size " << vertexCode.size() << std::endl;
	}
}