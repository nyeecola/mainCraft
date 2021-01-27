#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform VP {
	mat4 view_proj;
} camera;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inModelPosition;

layout(location = 1) out vec2 fragTexCoord;

void main() {
	mat4 modelMatrix = mat4(vec4(1.0, 0.0, 0.0,   0.0),
							vec4(0.0, 1.0, 0.0,   0.0),
							vec4(0.0, 0.0, 1.0,   0.0),
							vec4(inModelPosition, 1.0));

	gl_Position = camera.view_proj * modelMatrix * vec4(inPosition, 1.0);
	fragTexCoord = inTexCoord;
}

