#include <glm\glm.hpp>
#include <graphics_framework.h>
#include <memory>

using namespace std;
using namespace graphics_framework;
using namespace glm;

vector<mesh> meshes;
vector<texture> textures;
effect eff;
target_camera cam;

bool load_content() {
	// Construct geometry object
	geometry geom_triangle;
	geometry geom_quad;
	geometry geom_quad2;
	geometry geom_quad3;
	geometry geom_stopsign;

	//Quads
	geom_quad.set_type(GL_TRIANGLE_STRIP);
	geom_quad2.set_type(GL_TRIANGLE_STRIP);
	geom_quad3.set_type(GL_TRIANGLE_STRIP);
	//Triangle Fan
	geom_stopsign.set_type(GL_TRIANGLE_FAN);

	//Define positions
	//Triangle
	vector<vec3> triangle_positions{ vec3(5.0f, 1.0f, 0.0f), vec3(4.0f, -1.0f, 0.0f), vec3(6.0f, -1.0f, 0.0f) };
	//Quad
	vector<vec3> quad_positions{vec3(-4.0f, 1.0f, 0.0f),vec3(-5.0f, 1.0f, 0.0f),vec3(-4.0f, 0.0f, 0.0f),vec3(-5.0f, 0.0f, 0.0f)};
	//Quad 2
	vector<vec3> quad2_positions{ vec3(-4.0f, 0.0f, 0.0f),vec3(-5.0f, 0.0f, 0.0f),vec3(-4.0f, -1.0f, 0.0f),vec3(-5.0f, -1.0f, 0.0f) };
	//Quad 3
	vector<vec3> quad3_positions{ vec3(-4.0f, 2.0f, 0.0f),vec3(-5.0f, 2.0f, 0.0f),vec3(-4.0f, 1.0f, 0.0f),vec3(-5.0f, 1.0f, 0.0f) };
	//Triangle Fan
	vector<vec3> stopsign_positions{

		vec3(0.0f,0.0f,0.0f),
		vec3(1.0f,2.0f,0.0f),
		vec3(-1.0f,2.0f,0.0f),
		vec3(-2.0f,1.0f,0.0f),
		vec3(-2.0f,-1.0f,0.0f),
		vec3(-1.0f,-2.0f,0.0f),
		vec3(1.0f,-2.0f,0.0f),
		vec3(2.0f,-1.0f,0.0f),
		vec3(2.0f,1.0f,0.0f),
		vec3(1.0f,2.0f,0.0f)

	};

	// Define texture coordinates
	//Triangle
	vector<vec2> triangle_tex_coords{ vec2(0.5f, 1.0f), vec2(0.0f, 0.0f), vec2(1.0f,0.0f) };
	//Quad
	vector<vec2> quad_tex_coords{ vec2(1,1), vec2(0,1), vec2(1,0), vec2(0,0) };
	//Quad 2
	vector<vec2> quad2_tex_coords{ vec2(1,1), vec2(0,1), vec2(1,0), vec2(0,0) };
	//Quad 3
	vector<vec2> quad3_tex_coords{ vec2(1,1), vec2(0,1), vec2(1,0), vec2(0,0) };
	//Triangle Fan
	vector<vec2> stopsign_tex_coords{ vec2(0.5f,0.5f), vec2(0.65f,0.855f), vec2(0.35f,0.84f), vec2(0.143f,0.637f), vec2(0.143f,0.346f), vec2(0.35f,0.135f), vec2(0.66f,0.135f), vec2(0.873f,0.355f), vec2(0.862f, 0.652f), vec2(0.65f,0.855f) };

	//Add positions and texture coordinates to geometry
	geom_triangle.add_buffer(triangle_positions, BUFFER_INDEXES::POSITION_BUFFER);
	geom_triangle.add_buffer(triangle_tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);

	geom_quad.add_buffer(quad_positions, BUFFER_INDEXES::POSITION_BUFFER);
	geom_quad.add_buffer(quad_tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
	geom_quad2.add_buffer(quad2_positions, BUFFER_INDEXES::POSITION_BUFFER);
	geom_quad2.add_buffer(quad2_tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
	geom_quad3.add_buffer(quad3_positions, BUFFER_INDEXES::POSITION_BUFFER);
	geom_quad3.add_buffer(quad3_tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);

	geom_stopsign.add_buffer(stopsign_positions, BUFFER_INDEXES::POSITION_BUFFER);
	geom_stopsign.add_buffer(stopsign_tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);

	//Create and Add meshes to vector
	meshes.push_back(mesh(geom_triangle));
	meshes.push_back(mesh(geom_quad));
	meshes.push_back(mesh(geom_quad2));
	meshes.push_back(mesh(geom_quad3));
	meshes.push_back(mesh(geom_stopsign));

	// Load in texture shaders here
	eff.add_shader("27_Texturing_Shader/simple_texture.vert", GL_VERTEX_SHADER);
	eff.add_shader("27_Texturing_Shader/simple_texture.frag", GL_FRAGMENT_SHADER);
	// Build effect
	eff.build();

	// Load texture "textures/sign.jpg"
	textures.push_back(texture("textures/stonygrass.jpg"));
	textures.push_back(texture("textures/smiley.png"));
	textures.push_back(texture("textures/smiley.png")); // ??? this is not optimal
	textures.push_back(texture("textures/smiley.png")); // ???
	textures.push_back(texture("textures/sign.jpg"));

	// Set camera properties
	cam.set_position(vec3(0.0f, 0.0f, 15.0f));
	cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
	cam.set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);

	return true;
}

bool update(float delta_time) {
	// Update the camera
	cam.update(delta_time);
	return true;
}

bool render() {
	// Bind effect
	renderer::bind(eff);

	// Create MVP matrix
	int index = 0;
	for each (mesh m in meshes)
	{
		auto M = m.get_transform().get_transform_matrix();
		auto V = cam.get_view();
		auto P = cam.get_projection();
		auto MVP = P * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), // Location of uniform
							1,                               // Number of values - 1 mat4
							GL_FALSE,                        // Transpose the matrix?
							value_ptr(MVP));                 // Pointer to matrix data

		// Bind texture to renderer
		renderer::bind(textures.at(index), index);  // ???
		// Set the texture value for the shader here
		glUniform1i(eff.get_uniform_location("tex"), index);
		// Increment index
		index++;
		// Render the mesh
		renderer::render(m);
	}

	return true;
}

void main() {
	// Create application
	app application("27_Texturing_Shader");
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}