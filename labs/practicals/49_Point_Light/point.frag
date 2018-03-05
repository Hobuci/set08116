#version 440

// Point light information
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

// Material information
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Point light for the scene
uniform point_light light;
// Material for the object
uniform material mat;
// Eye position
uniform vec3 eye_pos;
// Texture
uniform sampler2D tex;

// Incoming position
layout(location = 0) in vec3 vertex_world_position;
// Incoming normal
layout(location = 1) in vec3 transformed_normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Get distance between point light and vertex
	float d = distance(light.position, vertex_world_position);
  // Calculate attenuation factor
	float attenuation = 1 / (light.constant + light.linear * d + (light.quadratic * pow(d, 2)));
  // Calculate light colour
	vec4 light_colour = light.light_colour * attenuation;
  // Calculate light dir
	vec3 light_dir = light.position - vertex_world_position;
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
	// Calculate diffuse component
	float k = max(dot(transformed_normal, light_dir), 0.0f);
	vec4 diffuse = k * (mat.diffuse_reflection * light_colour);

	// Calculate view direction
	vec3 view_direction = normalize(eye_pos - vec3(vertex_world_position));
	// Calculate half vector
	vec3 H = normalize(light_dir + view_direction);
	
	// Calculate specular component
	float k2 = pow(max(dot(transformed_normal, H), 0.0f), mat.shininess);
	vec4 specular = k2 * (mat.specular_reflection * light_colour);
	
	// Sample texture
	 vec4 tex_colour = texture(tex, tex_coord);
	 
	// Calculate primary colour component
	vec4 primary = mat.emissive + diffuse;
	// Calculate final colour - remember alpha
	colour = primary * tex_colour + specular;
	colour.a = 1.0;
  // *********************************
}