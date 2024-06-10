#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec3 fragNormal;
layout (location = 2) out vec3 fragUv;

layout (set = 0, binding = 0) uniform GlobalUbo {
	mat4 projectionViewMatrix;
	vec3 directionToLight;
} ubo;

layout (push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

const float AMBIENT = 0.02;

void main() {
	gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0);

	// the normal matrix is the transpose of the inverse of the model matrix
	// inverse() is expensive, so we construct the normal matrix by hand
	// note that the inverse of the rotation matrix is the same as its transpose
	// since the scale matrix is a diagonal matrix, its transpose is the same as the original
	// the inverse of the scale matrix is simply the reciprocal of each element
	// mat4 normalMatrix = transpose(inverse(push.modelMatrix))
	vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal); // convert normal to world space
	float lightIntensity = max(dot(normalWorldSpace, ubo.directionToLight), 0.0);

	fragColor = color * lightIntensity + AMBIENT;
	fragNormal = normal;
	fragUv = fragUv;
}