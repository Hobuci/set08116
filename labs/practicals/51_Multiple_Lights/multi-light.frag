#version 440

// Point light information
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

// Spot light data
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};

// Material data
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Point lights being used in the scene
uniform point_light points[4];
// Spot lights being used in the scene
uniform spot_light spots[5];
// Material of the object being rendered
uniform material mat;
// Position of the eye (camera)
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;

// Incoming position
layout(location = 0) in vec3 vertex_position;
// Incoming normal
layout(location = 1) in vec3 transformed_normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 final_colour;

// Point light calculation
vec4 calculate_point(in point_light point, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir,
                     in vec4 tex_colour) {
  // *********************************
  // Get distance between point light and vertex
	float d = distance(point.position, vertex_position);
  // Calculate attenuation factor
	float attenuation = point.constant + point.linear * d + (point.quadratic * pow(d, 2));
  // Calculate light colour
	vec4 light_colour = (1/attenuation) * point.light_colour;
  // Calculate light dir
	vec3 light_dir = normalize(point.position - vertex_position);
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
	// Calculate diffuse component
	float k = max(dot(transformed_normal, light_dir), 0.0f);
	vec4 diffuse = k * (mat.diffuse_reflection * light_colour);

	// Calculate half vector
	vec3 half_vec = normalize(light_dir + view_dir);
	
	// Calculate specular component
	float k2 = pow(max(dot(transformed_normal, half_vec), 0.0f), mat.shininess);
	vec4 specular = k2 * (mat.specular_reflection * light_colour);
	 
	// Calculate primary colour component
	vec4 primary = mat.emissive + diffuse;

	vec4 colour = primary * tex_colour + specular;
	colour.a = 1.0;
  // *********************************
  return colour;
}

// Spot light calculation
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir,
                    in vec4 tex_colour) {
  // *********************************
  // Calculate direction to the light
	vec3 light_dir = normalize(spot.position - vertex_position);
  // Calculate distance to light
	float d = distance(spot.position, vertex_position);
  // Calculate attenuation value
	float attenuation = spot.constant + spot.linear * d + (spot.quadratic * pow(d, 2));
  // Calculate spot light intensity
	float intensity = pow(max(dot(-spot.direction, light_dir), 0.0f), spot.power);
  // Calculate light colour
	vec4 light_colour = (intensity / attenuation) * spot.light_colour;
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  // Calculate diffuse component
	float k = max(dot(transformed_normal, light_dir), 0.0f);
	vec4 diffuse = k * (mat.diffuse_reflection * light_colour);

  // Calculate half vector
	vec3 half_vec = normalize(light_dir + view_dir);
	
  // Calculate specular component
	float k2 = pow(max(dot(transformed_normal, half_vec), 0.0f), mat.shininess);
	vec4 specular = k2 * (mat.specular_reflection * light_colour);

  // Calculate primary colour component
	vec4 primary = mat.emissive + diffuse;

	vec4 colour = primary * tex_colour + specular;
	colour.a = 1.0;
  // *********************************
  return colour;
}

void main() {

  final_colour = vec4(0.0, 0.0, 0.0, 1.0);
  // *********************************
  // Calculate view direction
	vec3 view_direction = normalize(eye_pos - vertex_position);
  // Sample texture
	vec4 tex_colour = texture(tex, tex_coord);
  // Sum point lights
	for (int i = 0; i < points.length(); i++)
	{
		final_colour += calculate_point(points[i], mat, vertex_position, transformed_normal, view_direction, tex_colour);
	}
  // Sum spot lights
	for (int i = 0; i < spots.length(); i++)
	{
		final_colour += calculate_spot(spots[i], mat, vertex_position, transformed_normal, view_direction, tex_colour);
	}
  // *********************************
}