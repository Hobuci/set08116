#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
effect eff;
texture tex;
target_camera cam;
spot_light light;

bool load_content() {
  // Create plane mesh
  meshes["plane"] = mesh(geometry_builder::create_plane());

  // Create scene
  meshes["box"] = mesh(geometry_builder::create_box());
  meshes["tetra"] = mesh(geometry_builder::create_tetrahedron());
  meshes["pyramid"] = mesh(geometry_builder::create_pyramid());
  meshes["disk"] = mesh(geometry_builder::create_disk(20));
  meshes["cylinder"] = mesh(geometry_builder::create_cylinder(20, 20));
  meshes["sphere"] = mesh(geometry_builder::create_sphere(20, 20));
  meshes["torus"] = mesh(geometry_builder::create_torus(20, 20, 1.0f, 5.0f));

  // Transform objects
  meshes["box"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
  meshes["box"].get_transform().translate(vec3(-10.0f, 2.5f, -30.0f));
  meshes["tetra"].get_transform().scale = vec3(4.0f, 4.0f, 4.0f);
  meshes["tetra"].get_transform().translate(vec3(-30.0f, 10.0f, -10.0f));
  meshes["pyramid"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
  meshes["pyramid"].get_transform().translate(vec3(-10.0f, 7.5f, -30.0f));
  meshes["disk"].get_transform().scale = vec3(3.0f, 1.0f, 3.0f);
  meshes["disk"].get_transform().translate(vec3(-10.0f, 11.5f, -30.0f));
  meshes["disk"].get_transform().rotate(vec3(half_pi<float>(), 0.0f, 0.0f));
  meshes["cylinder"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
  meshes["cylinder"].get_transform().translate(vec3(-25.0f, 2.5f, -25.0f));
  meshes["sphere"].get_transform().scale = vec3(2.5f, 2.5f, 2.5f);
  meshes["sphere"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));
  meshes["torus"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));
  meshes["torus"].get_transform().rotate(vec3(half_pi<float>(), 0.0f, 0.0f));

  // *********************************
  // Set materials
  // - all emissive is black
  // - all specular is white
  // - all shininess is 25
  material mat;
  mat.set_emissive(vec4(0, 0, 0, 1));
  mat.set_specular(vec4(1));
  mat.set_shininess(25.0f);
  // Red box
  mat.set_diffuse(vec4(1, 0, 0, 1));
  meshes["box"].set_material(mat);

  // Green tetra
  mat.set_diffuse(vec4(0, 1, 0, 1));
  meshes["tetra"].set_material(mat);

  // Blue pyramid
  mat.set_diffuse(vec4(0, 0, 1, 1));
  meshes["pyramid"].set_material(mat);

  // Yellow disk
  mat.set_diffuse(vec4(1, 1, 0, 1));
  meshes["disk"].set_material(mat);

  // Magenta cylinder
  mat.set_diffuse(vec4(1, 0, 1, 1));
  meshes["cylinder"].set_material(mat);

  // Cyan sphere
  mat.set_diffuse(vec4(0, 1, 1, 1));
  meshes["sphere"].set_material(mat);

  // White torus
  mat.set_diffuse(vec4(1, 1, 1, 1));
  meshes["torus"].set_material(mat);
  // *********************************

  // Load texture
  tex = texture("textures/checker.png");

  // *********************************
  // Set lighting values
  // Position (-25, 10, -10)
  light.set_position(vec3(-25.0f, 10.0f, -10.0f));
  // Light colour white
  light.set_light_colour(vec4(1));
  // Light direction to forward and down (normalized)
  light.set_direction(normalize(vec3(0.0f, -1.0f, 1.0f)));
  // Set range to 20
  light.set_range(20.0f);
  // Set power to 1
  light.set_power(1.0f);

  // Load in shaders
  eff.add_shader("50_Spot_Light/spot.vert", GL_VERTEX_SHADER);
  eff.add_shader("50_Spot_Light/spot.frag", GL_FRAGMENT_SHADER);

  // Build effect
  eff.build();
  // *********************************
  // Set camera properties
  cam.set_position(vec3(50.0f, 10.0f, 50.0f));
  cam.set_target(vec3(0.0f, 0.0f, 0.0f));
  cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
  return true;
}

bool update(float delta_time) {
  // Range of the spot light
  static float range = 20.0f;

  // *********************************
  if (glfwGetKey(renderer::get_window(), '1')) {
	  cam.set_position(vec3(50, 10, 50));
  }
  if (glfwGetKey(renderer::get_window(), '2')) {
	  cam.set_position(vec3(-50, 10, 50));
  }
  if (glfwGetKey(renderer::get_window(), '3')) {
	  cam.set_position(vec3(-50, 10, -50));
  }
  if (glfwGetKey(renderer::get_window(), '4')) {
	  cam.set_position(vec3(50, 10, -50));
  }
  // WSAD to move point light
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
	  light.move(vec3(0.0f, 0.0f, -1.0f));
  }
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
	  light.move(vec3(0.0f, 0.0f, 1.0f));
  }
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
	  light.move(vec3(-1.0f, 0.0f, 0.0f));
  }
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
	  light.move(vec3(1.0f, 0.0f, 0.0f));
  }

  // O and P to change range
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_O)) {
	  range += 5 * delta_time;
  }
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_P)) {
	  range -= 5 * delta_time;
  }
  // Cursor keys to rotate camera on X and Y axis
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT)) {
	  
  }
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_RIGHT)) {

  }
  // *********************************

  // Set range
  light.set_range(range);

  // Rotate the sphere
  meshes["sphere"].get_transform().rotate(vec3(0.0f, half_pi<float>(), 0.0f) * delta_time);

  cam.update(delta_time);

  return true;
}

bool render() {
  // Render meshes
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
    glUniformMatrix4fv(eff.get_uniform_location("MVP"), // Location of uniform
                       1,                               // Number of values - 1 mat4
                       GL_FALSE,                        // Transpose the matrix?
                       value_ptr(MVP));                 // Pointer to matrix data
    // *********************************
	// Set M matrix uniform
	glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
	// Set N matrix uniform - remember - 3x3 matrix
	glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
	// Bind material
	renderer::bind(m.get_material(), "mat");
	// Bind light
	renderer::bind(light, "light");
	// Bind texture
	renderer::bind(tex, 0);
	// Set tex uniform
	glUniform1i(eff.get_uniform_location("tex"), 0);
	// Set eye position- Get this from active camera
	glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(cam.get_position()));
	// Render mesh
	renderer::render(m);
    // *********************************
  }

  return true;
}

void main() {
  // Create application
  app application("50_Spot_Light");
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}