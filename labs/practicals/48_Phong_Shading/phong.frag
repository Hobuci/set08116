#version 440

// A directional light structure
struct directional_light {
  vec4 ambient_intensity;
  vec4 light_colour;
  vec3 light_dir;
};

// A material structure
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Directional light for the scene
uniform directional_light light;
// Material of the object
uniform material mat;
// Position of the camera
uniform vec3 eye_pos;
// Texture
uniform sampler2D tex;

// Incoming position
layout(location = 0) in vec3 world_position; //passed from vertex shader
// Incoming normal
layout(location = 1) in vec3 transformed_normal; //passed from vertex shader
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {

  // *********************************
  // Calculate ambient component
	vec4 ambient = mat.diffuse_reflection * light.ambient_intensity;

  // Calculate diffuse component
	float k = max(dot(transformed_normal, light.light_dir), 0.0f);
	vec4 diffuse = k * (mat.diffuse_reflection * light.light_colour);

  // Calculate view direction
	vec3 view_direction = normalize(eye_pos - world_position);
  // Calculate half vector
	vec3 H = normalize(light.light_dir + view_direction);

  // Calculate specular component
	float k2 = pow(max(dot(transformed_normal, H), 0.0f), mat.shininess);
	vec4 specular = k2 * (mat.specular_reflection * light.light_colour);

  // Sample texture
	 vec4 tex_colour = texture(tex, tex_coord);
  // Calculate primary colour component
	vec4 primary = mat.emissive + ambient + diffuse;
  // Calculate final colour - remember alpha
	colour = primary * tex_colour + specular;
	colour.a = 1.0;
  // *********************************
}