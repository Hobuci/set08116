#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
map<string, texture> textures;
geometry geom;
effect eff;
target_camera cam;


bool load_content() {
	// Add meshes
	meshes["plane"] = mesh(geometry_builder::create_plane());
	meshes["pyramid"] = mesh(geometry_builder::create_pyramid());
	meshes["sculpture"] = mesh(geometry("models/sculpture.obj"));
	//Translate meshes
	meshes["pyramid"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	meshes["pyramid"].get_transform().translate(vec3(0.0f, 0.0f, 0.0f));

	// Load in shaders
	eff.add_shader("coursework/texture.vert", GL_VERTEX_SHADER);
	eff.add_shader("coursework/texture.frag", GL_FRAGMENT_SHADER);
	// Build effect
	eff.build();

	//Load in textures
	textures["unTextured"] = texture("textures/checked.gif");
	textures["pyramid"] = texture("textures/stonygrass.jpg");
	textures["sculpture"] = texture("textures/stone.jpg");

	// Set camera properties
	cam.set_position(vec3(10.0f, 15.0f, 10.0f));
	cam.set_target(meshes["pyramid"].get_transform().position);
	cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	return true;
}


bool update(float delta_time) {
	// Update the camera
	cam.update(delta_time);
	return true;
}

bool render() {
	//Render meshes
	for (auto &e : meshes) {
		auto m = e.second;
		// Bind effect
		renderer::bind(eff);
		// Create MVP matrix
		auto M = m.get_transform().get_transform_matrix();
		auto V = cam.get_view();
		auto P = cam.get_projection();
		auto MVP = P * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		if (textures.count(e.first) == 0) 
		{//if no texture with the mesh's name exists, just bind unTextured
			renderer::bind(textures["unTextured"], 0);
		}
		else
		{
			renderer::bind(textures[e.first], 0);
		}

		glUniform1i(eff.get_uniform_location("tex"), 0);

		// Render mesh
		renderer::render(m);
	}
	return true;
}

void main() {
	// Create application
	app application("Graphics Coursework");
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}