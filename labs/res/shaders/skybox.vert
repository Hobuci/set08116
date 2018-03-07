#version 440

// The transformation matrix
uniform mat4 MVP;

// Incoming position
layout (location = 0) in vec3 position;
layout (location = 10) in vec2 tex_coord_in;
// Outgoing position
layout (location = 0) out vec2 tex_coord_out;

void main()
{
	// Transform the position into screen space
	gl_Position = MVP * vec4(position, 1.0);
	
	//output world position of vertex
		tex_coord_out = tex_coord_in;
}