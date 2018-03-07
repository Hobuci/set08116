#version 440

uniform samplerCube cubemapTex;

layout(location = 0) in vec2 tex_coord;

layout (location = 0) out vec4 colour;

void main()
{
	colour = texture(cubemapTex, position);
}