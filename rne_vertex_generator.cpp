#include "rne_vertex_generator.hpp"

#include <glm/common.hpp>

namespace rne {
	std::vector<RneModel::Vertex> RneVertexGenerator::triangle() {
		return std::vector<RneModel::Vertex> {
			{{0.0f, -0.5f, 0.5f}},
			{{0.5f, 0.5f, 0.5f}},
			{{-0.5f, 0.5f, 0.5f}}
		};
	}
	std::vector<RneModel::Vertex> RneVertexGenerator::sierpinski_triangle(const unsigned int &iter) {
		std::vector<RneModel::Vertex> triangles{
			{{0.0f, -1.0f, 0.5f}, {1.0f, 0.0f, 0.0f}},
			{{1.0f, 1.0f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-1.0f, 1.0f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};
		
		unsigned int current_triangles = 0;

		for (unsigned int i = 0; i < iter; i++) {
			current_triangles = static_cast<unsigned int>(triangles.size()) / 3;
			while (current_triangles > 0) {
				RneModel::Vertex v1 = triangles.front();
				triangles.erase(triangles.begin());
				RneModel::Vertex v2 = triangles.front();
				triangles.erase(triangles.begin());
				RneModel::Vertex v3 = triangles.front();
				triangles.erase(triangles.begin());
				glm::vec3 color = glm::vec3(1.0, 1.0, 1.0);

				// halve the size of the triangle
				RneModel::Vertex half_v1 = { v1.position * 0.5f, v1.color * 0.5f };
				RneModel::Vertex half_v2 = { v2.position * 0.5f, v2.color * 0.5f };
				RneModel::Vertex half_v3 = { v3.position * 0.5f, v3.color * 0.5f };

				// each triangle in clockwise starting from the top
				glm::vec3 t1_offset = v1.position - half_v1.position;
				glm::vec3 t1_color_offset = v1.color - half_v1.color;
				triangles.push_back({ half_v1.position + t1_offset, half_v1.color + t1_color_offset });
				triangles.push_back({ half_v2.position + t1_offset, half_v2.color + t1_color_offset });
				triangles.push_back({ half_v3.position + t1_offset, half_v3.color + t1_color_offset });

				glm::vec3 t2_offset = v2.position - half_v2.position;
				glm::vec3 t2_color_offset = v2.color - half_v2.color;
				triangles.push_back({ half_v1.position + t2_offset, half_v1.color + t2_color_offset });
				triangles.push_back({ half_v2.position + t2_offset, half_v2.color + t2_color_offset });
				triangles.push_back({ half_v3.position + t2_offset, half_v3.color + t2_color_offset });

				glm::vec3 t3_offset = v3.position - half_v3.position;
				glm::vec3 t3_color_offset = v3.color - half_v3.color;
				triangles.push_back({ half_v1.position + t3_offset, half_v1.color + t3_color_offset });
				triangles.push_back({ half_v2.position + t3_offset, half_v2.color + t3_color_offset });
				triangles.push_back({ half_v3.position + t3_offset, half_v3.color + t3_color_offset });
				

				current_triangles--;
			}
		}

		return triangles;
	};
}