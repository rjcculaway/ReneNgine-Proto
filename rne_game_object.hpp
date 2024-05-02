#pragma once

#include "rne_model.hpp"
// libs
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace rne {

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{1.0f, 1.0f, 1.0f};
		glm::vec3 rotation{};

		// translate * ry * rx * rz * scale
		glm::mat4 mat4() {
			// Tait-Bryan rotation (Y-X-Z), interpreted as an extrinsic rotation (read from right-to-left)
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);
			return glm::mat4{
				{
					scale.x * (c1 * c3 + s1 * s2 * s3),
					scale.x * (c2 * s3),
					scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				{
					scale.y * (c3 * s1 * s2 - c1 * s3),
					scale.y * (c2 * c3),
					scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					scale.z * (c2 * s1),
					scale.z * (-s2),
					scale.z * (c1 * c2),
					0.0f,
				},
				{translation.x, translation.y, translation.z, 1.0f} 
			};
		};
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
		TransformComponent transform{};

	private:
		RneGameObject(id_t objId) : id{ objId } {};

		id_t id;
	};
}