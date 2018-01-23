#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

geometry geom;
effect eff;
target_camera cam;

// Helper method - adds a triangle to geometry
void triangle(const vector<vec3> &points, vector<vec3> &positions, vector<vec4> &colours)
{
	positions.insert(positions.end(), points.begin(), points.end());

	for (auto i = 0; i < 3; ++i) {
		colours.push_back(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
}

//Sierpinski method - recursive
void sierpinski(const vector<vec3> &points, unsigned int count, vector<vec3> &positions, vector<vec4> &colours)
{
	if (count > 0 && points.size() == 3)
	{
		// Calculate new vertices to work on
		vec3 midp0 = (points[0] + points[1]) / 2.0f;
		vec3 midp1 = (points[1] + points[2]) / 2.0f;
		vec3 midp2 = (points[2] + points[0]) / 2.0f;

		//Divide triangles
		sierpinski({ points[0], midp0, midp2 }, count-1, positions, colours);
		sierpinski({ midp1, points[2], midp2 }, count-1, positions, colours);
		sierpinski({ midp0, points[1], midp1 }, count-1, positions, colours);
	}
	else
	{
		triangle(points, positions, colours);
	}
}

bool load_content() {
	// Required buffers
	vector<vec3> positions;
	vector<vec4> colours;
	sierpinski({ vec3(1.0f, -1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(-1.0f, -1.0f, 0.0f) }, 5, positions, colours);

	// Add to the geometry
	geom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	geom.add_buffer(colours, BUFFER_INDEXES::COLOUR_BUFFER);

	// Load in shaders
	eff.add_shader("shaders/basic.vert", GL_VERTEX_SHADER);
	eff.add_shader("shaders/basic.frag", GL_FRAGMENT_SHADER);
	// Build effect
	eff.build();

	// Set camera properties
	cam.set_position(vec3(3.0f, 3.0f, 5.0f));
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
	mat4 M(1.0f);
	auto V = cam.get_view();
	auto P = cam.get_projection();
	auto MVP = P * V * M;
	// Set MVP matrix uniform
	glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	// Render geometry
	renderer::render(geom);
	return true;
}

void main() {
	// Create application
	app application("22_Sierpinski_Gasket");
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}