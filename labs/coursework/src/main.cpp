/*
* Author: Richard Borbely
* 3D scene
* 40283185
* Note: Use numbers 1-3 to switch cameras
		Shadow rendering is ready to go, but for some reason the shadow map turns out to be very strange, therefore it fails.
		To build: place it in practicals folder and run CMAKE
*/
#include <glm\glm.hpp>
#include <graphics_framework.h>
#include <glm/ext.hpp>
using namespace std;
using namespace graphics_framework;
using namespace glm;

#pragma region Variables
map<string, mesh> meshes; map<string, texture> textures; map<string, texture> normal_maps;
//directional_light light; //not used in this project
vector<point_light> points(7); vector<spot_light> spots(5);
effect eff, sky_eff, normal_eff, shadow_eff;
cubemap cube_map;
free_camera cam_free;	//camNo 0
target_camera cam_target_1;	//camNo 1
target_camera cam_target_2;	//camNo 2
int activeCamNo = 1;	// Assigned number for active camera
double cursor_x = 0.0, cursor_y = 0.0;
float radius = 0.0f; // runtime radius for planets
float x, z; // planet trajectory
shadow_map shadow;
#pragma endregion

#pragma region Additional functions
bool cam_free_initialize() {
	// Capture initial mouse position
	glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);
	// Disable cursor
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return true;
}
bool cam_free_update(float delta_time)
{// Function for interacting and updating the free camera
	// The ratio of pixels to rotation - remember the fov
	static const float sh = static_cast<float>(renderer::get_screen_height());
	static const float sw = static_cast<float>(renderer::get_screen_width());
	static const double ratio_width = quarter_pi<float>() / sw;
	static const double ratio_height = (quarter_pi<float>() * (sh / sw)) / sh;
	double current_x;
	double current_y;

	// Get the current cursor position
	glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);

	// Calculate delta of cursor positions from last frame
	double delta_x = current_x - cursor_x;
	double delta_y = current_y - cursor_y;

	// Multiply deltas by ratios - gets actual change in orientation
	delta_x *= ratio_width;
	delta_y *= ratio_height;

	// Rotate cameras by delta
	cam_free.rotate(delta_x, -delta_y);

	// Camera translation vectors
	vec3 forward(.0f, .0f, .0f), backward(.0f, .0f, .0f), left(.0f, .0f, .0f), right(.0f, .0f, .0f), up(.0f, .0f, .0f), down(.0f, .0f, .0f);
	float speed(10.0f);

	// Use keyboard to move the camera - WSAD
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
		forward += vec3(.0f, .0f, 1.0f);
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
		backward += vec3(.0f, .0f, -1.0f);
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
		left += vec3(-1.0f, .0f, .0f);
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
		right += vec3(1.0f, .0f, .0f);
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_SPACE)) {
		up += vec3(.0f, 1.0f, .0f);
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_CONTROL)) {
		down += vec3(.0f, -1.0f, .0f);
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_SHIFT)) {
		speed += 20.0f;
	}

	// Move camera
	vec3 camTranslation = forward + backward + left + right + up + down;
	if (camTranslation != vec3(.0f, .0f, .0f)) {
		cam_free.move(camTranslation * delta_time * speed);
	}

	// Update the camera
	cam_free.update(delta_time);

	// Update cursor pos
	glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);

	return true;
}
mat4 getMVP(mat4 M)
{// Returns MVP matrix with defined mesh, using the currently active camera
	mat4 V;
	mat4 P;
	switch (activeCamNo)
	{
	case 0:
		V = cam_free.get_view();
		P = cam_free.get_projection();
		break;
	case 1:
		V = cam_target_1.get_view();
		P = cam_target_1.get_projection();
		break;
	case 2:
		V = cam_target_2.get_view();
		P = cam_target_2.get_projection();
		break;
	}
	return P * V * M;
}
vec3 getEyePos()
{// Returns active camera's position
	switch (activeCamNo)
	{
	case 0: return cam_free.get_position();
		break;
	case 1: return cam_target_1.get_position();
		break;
	case 2: return cam_target_2.get_position();
		break;
	}
}
bool renderSkybox()
{// Renders skybox

	// Disable depth test, depth mask, face culling
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	// Bind skybox effect
	renderer::bind(sky_eff);

	// Calculate MVP for the skybox
	auto MVP = getMVP(meshes["skybox"].get_transform().get_transform_matrix());
	// Set MVP matrix uniform
	glUniformMatrix4fv(sky_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	// Set cubemap uniform
	renderer::bind(cube_map, 0);
	glUniform1i(sky_eff.get_uniform_location("cubemapTex"), 0);

	// Render skybox
	renderer::render(meshes["skybox"]);

	// Re-Enable depth test, depth mask, face culling
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);

	return true;
}
bool renderNormalMap(pair<string, mesh> e, mat4 MVP, mat4 &LightProjectionMat)
{
	// Get mesh object
	auto m = e.second;
	// Bind normal_effect
	renderer::bind(normal_eff);
	// Set MVP matrix uniform
	glUniformMatrix4fv(normal_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	// Set M matrix uniform
	glUniformMatrix4fv(normal_eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(m.get_transform().get_transform_matrix()));
	// Set N matrix uniform
	glUniformMatrix3fv(normal_eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
		//SHADOW
		auto viewMatrix = shadow.get_view();
		// Multiply together with LightProjectionMat
		LightProjectionMat *= viewMatrix * m.get_transform().get_transform_matrix();
		// Set uniform
		glUniformMatrix4fv(eff.get_uniform_location("lightMVP"), 1, GL_FALSE, value_ptr(LightProjectionMat));

	// Bind material
	renderer::bind(m.get_material(), "mat");
	// Bind light
	//renderer::bind(light, "light");
	// Bind point lights
	renderer::bind(points, "points");
	// Bind spot lights
	renderer::bind(spots, "spots");
	// Bind texture
	// If 'textures' does not have a key with the same name as the mesh, then the mesh doesn't have a texture defined, bind default
	if (textures.count(e.first) == 0)
	{// Bind default
		renderer::bind(textures["unTextured"], 0);
	}
	else
	{// Bind corresponding texture
		renderer::bind(textures[e.first], 0);
	}
	// Set the texture value for the shader here
	glUniform1i(normal_eff.get_uniform_location("tex"), 0);
	// Bind normal_map
	renderer::bind(normal_maps[e.first], 1);
	// Set normal_map uniform
	glUniform1i(eff.get_uniform_location("normal_map"), 1);
	// Set eye position - Get this from active camera
	glUniform3fv(normal_eff.get_uniform_location("eye_pos"), 1, value_ptr(getEyePos()));
		//SHADOW
		// Bind shadow map texture - use texture unit 1
		renderer::bind(shadow.buffer->get_depth(), 2);
		// Set the shadow_map uniform
		glUniform1i(eff.get_uniform_location("shadow_map"), 2);

	// Render geometry
	renderer::render(m);
	return true;
}
bool renderShadows(mat4 &LightProjectionMat)
{
	// Set render target to shadow map
	renderer::set_render_target(shadow);
	// Clear depth buffer bit
	glClear(GL_DEPTH_BUFFER_BIT);
	// Set face cull mode to front
	glCullFace(GL_FRONT);

	// Proj Mat with a field of view of 90.
	LightProjectionMat = perspective<float>(90.f, renderer::get_screen_aspect(), 0.1f, 1000.f);

	// Bind shader
	renderer::bind(shadow_eff);
	// Render meshes
	for (auto &e : meshes) {
		auto m = e.second;
		// Create MVP matrix
		auto M = m.get_transform().get_transform_matrix();

		// View matrix taken from shadow map
		auto V = shadow.get_view();

		auto MVP = LightProjectionMat * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), // Location of uniform
			1,                                      // Number of values - 1 mat4
			GL_FALSE,                               // Transpose the matrix?
			value_ptr(MVP));                        // Pointer to matrix data
													
		// Render mesh
		renderer::render(m);
	}

	// Set render target back to the screen
	renderer::set_render_target();
	// Set face cull mode to back
	glCullFace(GL_BACK);

	return true;
}
void movePlanets(float delta_time)
{
	meshes["planet1"].get_transform().rotate(vec3(0, quarter_pi<float>() * delta_time / 2, 0));
	radius += 0.03f; // this affects the radius (very sensitive)
	x = sinf(radius) / 10; // division affects the speed
	z = cosf(radius) / 10;

	meshes["planet2"].get_transform().translate(vec3(x, 0, z));
	meshes["planet2"].get_transform().rotate(vec3(0, 0, quarter_pi<float>() * delta_time));
	meshes["planet3"].get_transform().translate(vec3(0, x * 0.4f, 0)); //add sine wave to Y axes
	meshes["planet6"].get_transform().translate(vec3(-x * 0.6f, 0, z * 0.6f)); // reverse direction
	meshes["planet6"].get_transform().rotate(vec3(0, 0, half_pi<float>() * delta_time));
	meshes["planet7"].get_transform().translate(vec3(x * 0.2f, 0, z * 0.2f)); //add sine wave to all axes
	spots[1].set_position(meshes["planet2"].get_transform().position);
	spots[2].set_position(meshes["planet3"].get_transform().position + meshes["planet2"].get_transform().position + vec3(0.5f, 0, -1));
	spots[3].set_position(meshes["planet6"].get_transform().position);
}
#pragma endregion

#pragma region Main functions
bool load_content() {
// MESHES
	// Sphere to test location of lights
	//meshes["locationTest"] = mesh(geometry_builder::create_sphere(10, 10));
	//meshes["locationTest"].get_transform().scale = vec3(0.2f);
	// Create shadow map
	shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());
	// Create meshes
	meshes["skybox"] = mesh(geometry_builder::create_box());
	meshes["grass"] = mesh(geometry_builder::create_plane());
	meshes["statue"] = mesh(geometry("coursework/statue.obj"));
	meshes["gate"] = mesh(geometry("coursework/gate.obj"));
	meshes["gate2"] = meshes["gate"];
	meshes["water"] = mesh(geometry_builder::create_disk(10));
	meshes["fountain"] = mesh(geometry_builder::create_torus(45, 6, 0.5f, 6.0f));
	meshes["hedge"] = mesh(geometry_builder::create_box());
	meshes["hedge2"] = mesh(geometry_builder::create_box());
	meshes["hedge3"] = mesh(geometry_builder::create_box());
	meshes["hedge4"] = mesh(geometry_builder::create_box());
	meshes["lampp"] = mesh(geometry("coursework/lamp_post.obj"));
	meshes["lampp2"] = meshes["lampp"];
	meshes["bench"] = mesh(geometry("coursework/bench.obj"));
	meshes["planet1"] = mesh(geometry_builder::create_sphere(25, 25));
	meshes["planet2"] = mesh(geometry_builder::create_sphere(20, 20));
	meshes["planet3"] = mesh(geometry_builder::create_sphere(20, 20));
	meshes["planet4"] = mesh(geometry_builder::create_sphere(20, 20));
	meshes["planet5"] = mesh(geometry_builder::create_sphere(20, 20));
	meshes["planet6"] = mesh(geometry_builder::create_sphere(20, 20));
	meshes["planet7"] = mesh(geometry_builder::create_sphere(20, 20));
	// Transform objects
	meshes["skybox"].get_transform().scale = vec3(500);
	meshes["grass"].get_transform().scale = vec3(0.6f);
	meshes["statue"].get_transform().scale = vec3(0.5f);
	meshes["statue"].get_transform().translate(vec3(0.0f, 3.0f, 0.0f));
	meshes["gate"].get_transform().scale = vec3(0.02f);
	meshes["gate"].get_transform().translate(vec3(-12, 2, -5));
	meshes["gate2"].get_transform().scale = vec3(0.02f);
	meshes["gate2"].get_transform().translate(vec3(12, 2, 17));
	meshes["water"].get_transform().scale = vec3(12, 1, 12);
	meshes["water"].get_transform().translate(vec3(0, 0.5f, 7));
	meshes["fountain"].get_transform().translate(vec3(0, 0.2f, 7));
	meshes["hedge"].get_transform().scale = vec3(25, 5, 1);
	meshes["hedge"].get_transform().translate(vec3(0, 2, -8));
	meshes["hedge2"].get_transform().scale = vec3(22, 5, 1);
	meshes["hedge2"].get_transform().translate(vec3(-12, 2, 8.5f));
	meshes["hedge2"].get_transform().rotate(vec3(0, half_pi<float>(), 0));
	meshes["hedge2"].get_transform().rotate(vec3(0, half_pi<float>(), 0)); //my laptop doesn't rotate unless its duplicated
	meshes["hedge3"].get_transform().scale = vec3(22, 5, 1);
	meshes["hedge3"].get_transform().translate(vec3(12, 2, 3.5));
	meshes["hedge3"].get_transform().rotate(vec3(0, half_pi<float>(), 0));
	meshes["hedge3"].get_transform().rotate(vec3(0, half_pi<float>(), 0)); //my laptop doesn't rotate unless its duplicated
	meshes["hedge4"].get_transform().scale = vec3(25, 5, 1);
	meshes["hedge4"].get_transform().translate(vec3(0, 2, 20));
	meshes["lampp"].get_transform().scale = vec3(0.35);
	meshes["lampp"].get_transform().translate(vec3(-11, 0, -2));
	meshes["lampp2"].get_transform().scale = vec3(0.35);
	meshes["lampp2"].get_transform().translate(vec3(3, 0, 18));
	meshes["lampp2"].get_transform().rotate(vec3(0, half_pi<float>(), 0));
	meshes["lampp2"].get_transform().rotate(vec3(0, half_pi<float>(), 0)); //my laptop doesn't rotate unless its duplicated
	meshes["bench"].get_transform().scale = vec3(0.02);
	meshes["bench"].get_transform().translate(vec3(0, 0, 18));
	meshes["bench"].get_transform().rotate(vec3(0, pi<float>(), 0));
	meshes["bench"].get_transform().rotate(vec3(0, pi<float>(), 0)); //my laptop doesn't rotate unless its duplicated
	meshes["planet1"].get_transform().scale = vec3(0.6f);
	meshes["planet1"].get_transform().translate(vec3(0, 1.5f, 7));
	meshes["planet2"].get_transform().scale = vec3(0.3f);
	meshes["planet2"].get_transform().translate(vec3(-3.5f, 0.9f, 7.5f));
	meshes["planet3"].get_transform().scale = vec3(0.85f);
	meshes["planet3"].get_transform().translate(vec3(1.5f, 0, 0));
	meshes["planet4"].get_transform().scale = vec3(0.25f);
	meshes["planet4"].get_transform().translate(vec3(0.8f, 2.55f, 0.6f));
	meshes["planet5"].get_transform().scale = vec3(0.3f);
	meshes["planet5"].get_transform().translate(vec3(0.3f, 2.65f, 0.75f));
	meshes["planet6"].get_transform().scale = vec3(0.15f);
	meshes["planet6"].get_transform().translate(vec3(2.5f, 1.2f, 7.5f));
	meshes["planet7"].get_transform().scale = vec3(0.9f);
	meshes["planet7"].get_transform().translate(vec3(0, 2, 0));

// LIGHTING
	// Set materials
	material mat;
	mat.set_emissive(vec4(0, 0, 0, 1));

	// Fountain
	mat.set_diffuse(vec4(0.6f, 0.6f, 0.6f, 1));
	mat.set_specular(vec4(1));
	mat.set_shininess(10.0f);
	meshes["fountain"].set_material(mat);
	// Water
	mat.set_diffuse(vec4(0.5f, 0.5f, 0.5f, 1));
	mat.set_specular(vec4(0.5f));
	mat.set_shininess(5);
	meshes["water"].set_material(mat);
	// Grass
	mat.set_diffuse(vec4(0.3f, 0.5f, 0.3f, 1));
	mat.set_specular(vec4(0));
	mat.set_shininess(5.0f);
	meshes["grass"].set_material(mat);
	// Lamp Post
	mat.set_diffuse(vec4(0.5f, 0.5f, 0.5f, 1));
	mat.set_specular(vec4(0.8f));
	mat.set_shininess(5.0f);
	meshes["lampp"].set_material(mat);
	meshes["lampp2"].set_material(mat);
	// Bench
	mat.set_diffuse(vec4(1, 1, 1, 1));
	mat.set_specular(vec4(0.1f));
	mat.set_shininess(5.0f);
	meshes["bench"].set_material(mat);
	// Gate
	mat.set_diffuse(vec4(1, 1, 1, 1));
	mat.set_specular(vec4(1));
	mat.set_shininess(5);
	meshes["gate"].set_material(mat);
	meshes["gate2"].set_material(mat);
	// Hedge
	mat.set_diffuse(vec4(0.4f, 0.5f, 0.4f, 1));
	mat.set_specular(vec4(0));
	mat.set_shininess(5.0f);
	meshes["hedge"].set_material(mat);
	meshes["hedge2"].set_material(mat);
	meshes["hedge3"].set_material(mat);
	meshes["hedge4"].set_material(mat);
	// Planet1
	mat.set_emissive(vec4(0.1f, 0.1f, 0.2f, 1));
	mat.set_diffuse(vec4(1, 1, 1, 1));
	mat.set_specular(vec4(0));
	mat.set_shininess(5.0f);
	meshes["planet1"].set_material(mat);
	// Planet2
	mat.set_diffuse(vec4(.5f, .1f, .5f, 1));
	mat.set_specular(vec4(0));
	mat.set_shininess(5.0f);
	meshes["planet2"].set_material(mat);
	// Planet3
	mat.set_emissive(vec4(0.15f, 0.05f, 0.05f, 1));
	mat.set_diffuse(vec4(1, 1, 1, 1));
	mat.set_specular(vec4(0.5f));
	mat.set_shininess(25.0f);
	meshes["planet3"].set_material(mat);
	// Planet4
	mat.set_emissive(vec4(0.05f, 0.1f, 0.05f, 1));
	mat.set_diffuse(vec4(1, 0.1f, 0, 1));
	mat.set_specular(vec4(0.5f));
	mat.set_shininess(5.0f);
	meshes["planet4"].set_material(mat);
	// Planet5
	mat.set_emissive(vec4(.09f, .09f, .09f, 1));
	mat.set_diffuse(vec4(.5f, .5f, .5f, 1));
	mat.set_specular(vec4(0.1f));
	mat.set_shininess(5.0f);
	meshes["planet5"].set_material(mat);
	// Planet6
	mat.set_diffuse(vec4(0, 0, 0, 1));
	mat.set_specular(vec4(1));
	mat.set_shininess(50.0f);
	meshes["planet6"].set_material(mat);
	// Planet2
	mat.set_diffuse(vec4(.1f, .5f, .1f, 1));
	mat.set_specular(vec4(1));
	mat.set_shininess(25.0f);
	meshes["planet7"].set_material(mat);
	// Statue
	mat.set_diffuse(vec4(0.3f, 0.25f, 0.2f, 1));
	mat.set_specular(vec4(0));
	mat.set_shininess(1);
	meshes["statue"].set_material(mat);
	
	// Set lighting values
	// Point 0 - lamp post 1
	points[0].set_position(meshes["lampp"].get_transform().position + vec3(2.2f, 6, 0));
	points[0].set_light_colour(vec4(1, 0.8f, 0, 1.0f));
	points[0].set_range(10.0f);
	// Point 1 - lamp post 1
	points[1].set_position(meshes["lampp"].get_transform().position + vec3(1.4f, 6, 0.4f));
	points[1].set_light_colour(vec4(1, 0.8f, 0, 1.0f));
	points[1].set_range(10.0f);
	// Point 2 - lamp post 1
	points[2].set_position(meshes["lampp"].get_transform().position + vec3(1.4f, 6, -0.4f));
	points[2].set_light_colour(vec4(1, 0.8f, 0, 1.0f));
	points[2].set_range(10.0f);
	// Point 3 - lamp post 2
	points[3].set_position(meshes["lampp2"].get_transform().position + vec3(0, 6, -1.2f));
	points[3].set_light_colour(vec4(1, 0.8f, 0, 1.0f));
	points[3].set_range(10.0f);
	// Point 4 - lamp post 2
	points[4].set_position(meshes["lampp2"].get_transform().position + vec3(-0.5f, 6, -2));
	points[4].set_light_colour(vec4(1, 0.8f, 0, 1.0f));
	points[4].set_range(10.0f);
	// Point 5 - lamp post 2
	points[5].set_position(meshes["lampp2"].get_transform().position + vec3(0.5f, 6, -2));
	points[5].set_light_colour(vec4(1, 0.8f, 0, 1.0f));
	points[5].set_range(10.0f);

	// Spot 0 - under planet 1
	spots[0].set_position(vec3(meshes["planet1"].get_transform().position - vec3(0, 0, 0)));
	spots[0].set_light_colour(vec4(0.2f, 0.2f, 1, 1));
	spots[0].set_direction(normalize(vec3(0, -1, 0)));
	spots[0].set_range(10.0f);
	spots[0].set_power(1);
	// Spot 1 - under planet 2
	spots[1].set_light_colour(vec4(0.6f, 0, 0.8f, 1));
	spots[1].set_direction(normalize(vec3(0, -1, 0)));
	spots[1].set_range(5.0f);
	spots[1].set_power(1);
	// Spot 2 - under planet 3
	spots[2].set_light_colour(vec4(0.7f, 0, 0, 1));
	spots[2].set_direction(normalize(vec3(0, -1, 0)));
	spots[2].set_range(7);
	spots[2].set_power(1);
	// Spot 3 - under planet 4
	spots[3].set_light_colour(vec4(0.4f, 0.4f, 0.4f, 1));
	spots[3].set_direction(normalize(vec3(0, -1, 0)));
	spots[3].set_range(5.0f);
	spots[3].set_power(1);
	// Spot 4 - above the whole scene
	spots[4].set_position(vec3(0, 50, 0));
	spots[4].set_light_colour(vec4(0.7f, 0.7f, 0.9f, 1.0f));
	spots[4].set_direction(normalize(vec3(0, -1, 0)));
	spots[4].set_range(100.0f);
	spots[4].set_power(0.3f);
	// Directional
	/*
	light.set_ambient_intensity(vec4(1, 1, 1, 1.0f));
	light.set_light_colour(vec4(1, 1, 1, 1));
	light.set_direction(vec3(-1, -1, -1));
	*/
	// Sphere to test location of lights
	//meshes["locationTest"].get_transform().position = vec3(15, 25, 25);

// SHADERS
	// Load in main shaders
	eff.add_shader("coursework/shader.vert", GL_VERTEX_SHADER);
	eff.add_shader("coursework/shader.frag", GL_FRAGMENT_SHADER);
	eff.add_shader("coursework/part_spot.frag", GL_FRAGMENT_SHADER);
	eff.add_shader("coursework/part_point.frag", GL_FRAGMENT_SHADER);
	eff.add_shader("coursework/part_shadow.frag", GL_FRAGMENT_SHADER);
	//eff.add_shader("coursework/part_direction.frag", GL_FRAGMENT_SHADER);
	// Build effect
	eff.build();
	// Load in shaders for normal map
	normal_eff.add_shader("coursework/normal_shader.vert", GL_VERTEX_SHADER);
	normal_eff.add_shader("coursework/normal_shader.frag", GL_FRAGMENT_SHADER);
	normal_eff.add_shader("coursework/part_normal.frag", GL_FRAGMENT_SHADER);
	normal_eff.add_shader("coursework/part_spot.frag", GL_FRAGMENT_SHADER);
	normal_eff.add_shader("coursework/part_point.frag", GL_FRAGMENT_SHADER);
	normal_eff.add_shader("coursework/part_shadow.frag", GL_FRAGMENT_SHADER);
	//normal_eff.add_shader("coursework/part_direction.frag", GL_FRAGMENT_SHADER);
	normal_eff.build();
	// Load in skybox shaders
	sky_eff.add_shader("coursework/skybox.vert", GL_VERTEX_SHADER);
	sky_eff.add_shader("coursework/skybox.frag", GL_FRAGMENT_SHADER);
	sky_eff.build();
	// Load in shadow shaders
	shadow_eff.add_shader("coursework/shadow.vert", GL_VERTEX_SHADER);
	shadow_eff.add_shader("coursework/shadow.frag", GL_FRAGMENT_SHADER);
	shadow_eff.build();

// TEXTURES
	// Load in textures
	array<string, 6> skybox_tex = { "coursework/miramar_ft.png", "coursework/miramar_bk.png", "coursework/miramar_up.png",
									"coursework/miramar_dn.png", "coursework/miramar_rt.png", "coursework/miramar_lf.png" };
	normal_maps["gate"] = texture("coursework/gate_normal.png"); normal_maps["gate2"] = normal_maps["gate"];
	normal_maps["statue"] = texture("coursework/statue_normal.jpg");
	textures["unTextured"] = texture("coursework/checked.gif");
	textures["grass"] = texture("coursework/grassHD.jpg");
	textures["water"] = texture("coursework/water.jpg");
	textures["fountain"] = texture("coursework/marble.jpg");
	textures["gate"] = texture("coursework/gate.png"); textures["gate2"] = textures["gate"];
	textures["hedge"] = texture("coursework/hedge.png"); textures["hedge2"] = textures["hedge"]; textures["hedge3"] = textures["hedge"]; textures["hedge4"] = textures["hedge"];
	textures["lampp"] = texture("coursework/metal.jpg"); textures["lampp2"] = textures["lampp"];
	textures["bench"] = texture("coursework/wood.jpg");
	textures["planet1"] = texture("coursework/planet1.jpg"); textures["planet2"] = textures["planet1"]; textures["planet3"] = textures["planet1"];
																textures["planet4"] = textures["planet1"]; textures["planet7"] = textures["planet1"];
	textures["planet5"] = texture("coursework/planet2.jpg"); textures["planet6"] = textures["planet5"];
	textures["statue"] = texture("coursework/statue_tex.jpg");
	// Add textures to cubemap
	cube_map = cubemap(skybox_tex);
// CAMERAS
	// Set camera properties
	cam_free.set_position(vec3(0.0f, 5.0f, 10.0f));
	cam_free.set_target(vec3(0.0f, 0.0f, 0.0f));
	cam_free.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	cam_target_1.set_position(vec3(0.0f, 3.0f, 20.0f));
	cam_target_1.set_target(vec3(0.0f, 0.0f, 2.0f));
	cam_target_1.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	cam_target_2.set_target(meshes["planet1"].get_transform().position);
	cam_target_2.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	return true;
}
bool update(float delta_time) {
	// Camera change
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_1)){
		activeCamNo = 0;
		cam_free_initialize();
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_2)){
		activeCamNo = 1;
		// Re-Enable cursor
		glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_3)) {
		activeCamNo = 2;
		// Re-Enable cursor
		glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	// Update active camera
	switch (activeCamNo)
	{
	case 0: cam_free_update(delta_time);
			// Keep camera in the centre of skybox
			meshes["skybox"].get_transform().position = cam_free.get_position();
		break;
	case 1: cam_target_1.update(delta_time);
			// Keep camera in the centre of skybox
			meshes["skybox"].get_transform().position = cam_target_1.get_position();
		break;
	case 2: cam_target_2.update(delta_time);
			// Keep camera in the centre of skybox
			meshes["skybox"].get_transform().position = cam_target_2.get_position();
			cam_target_2.set_position(meshes["planet2"].get_transform().position);
		break;
	}

	movePlanets(delta_time);

	//shadow.light_position = vec3(15, 25, 25);
	//shadow.light_dir = vec3(-1, -1, 1);

	if (glfwGetKey(renderer::get_window(), 'I') == GLFW_PRESS)
		shadow.buffer->save("test.png");

	return true;
}
bool render() {
// SKYBOX
	renderSkybox();
// SHADOW
	mat4 LightProjectionMat = mat4(0);
	renderShadows(LightProjectionMat);
// MESHES
	for (auto &e : meshes) 
	{
		// Get mesh object
		auto m = e.second;
		auto MVP = mat4(0);
		
		// Transformation inheritance
		// Each planet has their own transformation but 3 and 7 inherit from other ones
		if (e.first == "planet3")
		{// If planet3 comes, apply planet2's transform
			MVP = getMVP(m.get_transform().get_transform_matrix() * meshes["planet2"].get_transform().get_transform_matrix());
		}
		else if (e.first == "planet7")
		{// If planet7 comes, apply planet2's and planet3's transform
			MVP = getMVP(m.get_transform().get_transform_matrix() * meshes["planet2"].get_transform().get_transform_matrix() * meshes["planet3"].get_transform().get_transform_matrix());
		}
		else 
		{// Create MVP matrix
			MVP = getMVP(m.get_transform().get_transform_matrix());
		}
		if (normal_maps.count(e.first) == 1)
		{//if the mesh has a normal map, render it
			renderNormalMap(e, MVP, LightProjectionMat);
		}
		else
		{//render without normal map
			// Bind effect
			renderer::bind(eff);
			// Set MVP matrix uniform
			glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
			// Set M matrix uniform
			glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(m.get_transform().get_transform_matrix()));
			// Set N matrix uniform
			glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
				//SHADOW
				// viewmatrix from the shadow map
				auto viewMatrix = shadow.get_view();
				// Multiply together with LightProjectionMat
				LightProjectionMat *= viewMatrix * m.get_transform().get_transform_matrix();
				// Set uniform
				glUniformMatrix4fv(eff.get_uniform_location("lightMVP"), 1, GL_FALSE, value_ptr(LightProjectionMat));

			// Bind material
			renderer::bind(m.get_material(), "mat");
			// Bind light
			//renderer::bind(light, "light");
			// Bind point lights
			renderer::bind(points, "points");
			// Bind spot lights
			renderer::bind(spots, "spots");
			// Bind texture
			// If 'textures' does not have a key with the same name as the mesh, then the mesh doesn't have a texture defined, bind default
			if (textures.count(e.first) == 0)
			{// Bind default
				renderer::bind(textures["unTextured"], 0);
			}
			else
			{// Bind corresponding texture
				renderer::bind(textures[e.first], 0);
			}
			// Set the texture value for the shader here
			glUniform1i(eff.get_uniform_location("tex"), 0);
			// Set eye position - Get this from active camera
			glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(getEyePos()));
				//SHADOW
				// Bind shadow map texture - use texture unit 1
				renderer::bind(shadow.buffer->get_depth(), 1);
				// Set the shadow_map uniform
				glUniform1i(eff.get_uniform_location("shadow_map"), 1);

			// Render geometry
			renderer::render(m);
		}
	}
	return true;
}
void main() {
	// Create application
	app application("Graphics Coursework");
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_initialise(cam_free_initialize);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}
#pragma endregion