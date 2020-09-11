#version 450
#extension GL_BaseInstanceARB_separate_shader_objects : enable



out gl_PerVertex{
	vec4 gl_Position;
};

layout(binding = 0) uniform MATRICES{
	mat4 model;
	mat4 view;
	mat4 proj;
} ubo;

layout(location = 0) in vec3 pos;

void main(){
	gl_Position = ubo.proj * ubo.view * ubo.proj * vec4(pos, 1);
}