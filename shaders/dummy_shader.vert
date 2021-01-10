#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform MVP {
	mat4 model;
	mat4 view_proj;
} camera;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 1) out vec2 fragTexCoord;

void main() {
	gl_Position = camera.view_proj * camera.model * vec4(inPosition, 0.0, 1.0);
	fragTexCoord = inTexCoord;
}

