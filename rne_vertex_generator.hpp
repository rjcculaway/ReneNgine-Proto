#pragma once

#include "rne_model.hpp"
#include <vector>

namespace rne {
	class RneVertexGenerator {
	public:
		static std::vector<RneModel::Vertex> triangle();
		static std::vector<RneModel::Vertex> sierpinski_triangle(const unsigned int &iter);
	};
};