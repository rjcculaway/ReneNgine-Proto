#pragma once

#include "rne_model.hpp"

#include <memory>

namespace rne {

	struct Transform2dComponent {
		glm::vec2 translation{};
		glm::vec2 scale{1.0f, 1.0f};
		float rotation{};

		glm::mat2 mat2() {
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMatrix{ { c, s }, {-s, c} };


			glm::mat2 scaleMatrix{ {scale.x, 0.0f}, {0.0f, scale.y} };
			return rotMatrix * scaleMatrix; 
		}
	};

	class RneGameObject {
	public:
		using id_t = unsigned int;

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
		Transform2dComponent transform2d{};

	private:
		RneGameObject(id_t objId) : id{ objId } {};

		id_t id;
	};
}