#version 450
#extension GL_BaseInstanceARB_separate_shader_objects : enable



out gl_PerVertex{
	vec4 gl_Position;
};

layout(binding = 0) uniform matrices{
	mat4 modelViewProjection;
};

layout(location = 0) in vec3 pos;

void main(){
	gl_Position = modelViewProjection * vec4(pos, 1);
}