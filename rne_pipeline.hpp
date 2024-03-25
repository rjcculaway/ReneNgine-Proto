#pragma once

#include <string>
#include <vector>

namespace rne {
	class RnePipeline {
	public:
		RnePipeline(const std::string& vertexFilePath, const std::string& fragmentFilePath);
	private:
		static std::vector<char> readFile(const std::string& fileName);
		void createGraphicsPipeline(const std::string& vertexFilePath, const std::string& fragmentFilePath);

	};
}