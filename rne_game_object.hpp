#pragma once

#include "rne_model.hpp"
// libs
#include <glm/gtc/matrix_transform.hpp>

// standard
#include <memory>
#include <unordered_map>

namespace rne {

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{1.0f, 1.0f, 1.0f};
		glm::vec3 rotation{};

		// translate * ry * rx * rz * scale
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	class RneGameObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, RneGameObject>;

		static RneGameObject createGameObject() {
			static id_t currentId = 0;

			return RneGameObject(currentId++);
		}

		// Remove copy constructor
		RneGameObject(const RneGameObject&) = delete;
		RneGameObject& operator=(const RneGameObject&) = delete;
		// Retain move
		RneGameObject(RneGameObject&&) = default;
		RneGameObject& operator=(RneGameObject&&) = default;

		id_t getId() { return id; }

		std::shared_ptr<RneModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};

	private:
		RneGameObject(id_t objId) : id{ objId } {};

		id_t id;
	};
}