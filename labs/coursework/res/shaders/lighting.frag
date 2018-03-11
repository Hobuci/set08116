#version 440

// Directional light structure
struct directional_light {
  vec4 ambient_intensity;
  vec4 light_colour;
  vec3 light_dir;
};

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

// A material structure
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Directional light information
//uniform directional_light light;
// Point lights being used in the scene
uniform point_light points[7];
// Spot lights being used in the scene
uniform spot_light spots[1];
// Material of the object being rendered
uniform material mat;
// Position of the eye
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 1) in vec3 normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

vec4 calculate_direction(in directional_light light, in material mat, in vec3 normal, in vec3 view_dir, in vec4 tex_colour)
{
	// Calculate ambient component
	vec4 ambient = mat.emissive * light.ambient_intensity;
	// Calculate diffuse component :  (diffuse reflection * light_colour) *  max(dot(normal, light direction), 0)
	vec4 diffuse = (mat.diffuse_reflection * light.light_colour) * max(dot(normal, light.light_dir), 0);
	// Calculate normalized half vector 
	vec3 half_vector = normalize(light.light_dir + view_dir);
	// Calculate specular component : (specular reflection * light_colour) * (max(dot(normal, half vector), 0))^mat.shininess
	vec4 specular = (mat.specular_reflection * light.light_colour) * pow(max(dot(normal, half_vector), 0), mat.shininess);

	// Calculate colour to return
	vec4 colour = ((mat.emissive + ambient + diffuse) * tex_colour) + specular;
	colour.a = 1.0;

	return colour;
}

vec4 calculate_point(in point_light point, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour)
{
	// Get distance between point light and vertex
	float d = distance(point.position, position);
	// Calculate attenuation factor : constant + (linear * d) + (quadratic * d * d)
	float attenuation = point.constant + point.linear * d + (point.quadratic * pow(d, 2));
	// Calculate light colour : light_colour / attenuation
	vec4 light_colour = (1/attenuation) * point.light_colour;
	// Set colour alpha to 1.0
	light_colour.a = 1.0;
	// Calculate light dir
	vec3 light_dir = normalize(point.position - position);
	// Phong shading
	vec4 diffuse = (mat.diffuse_reflection * light_colour) * max(dot(normal, light_dir), 0);
	vec3 half_vector = normalize(light_dir + view_dir);
	vec4 specular = (mat.specular_reflection * light_colour) * pow(max(dot(normal, half_vector), 0), mat.shininess);
	vec4 primary = mat.emissive + diffuse;
	vec4 colour = primary * tex_colour + specular;
	colour.a = 1.0;
	
	return colour;
}

vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour)
{
	// Calculate direction to the light
	vec3 light_dir = normalize(spot.position - position);
	// Calculate distance to light
	float d = distance(spot.position, position);
	// Calculate attenuation value :  (constant + (linear * d) + (quadratic * d * d)
	float attenuation = spot.constant + spot.linear * d + (spot.quadratic * pow(d, 2));
	// Calculate spot light intensity :  (max( dot(light_dir, -direction), 0))^power
	float intensity = pow(max(dot(-spot.direction, light_dir), 0.0f), spot.power);
	// Calculate light colour:  (intensity / attenuation) * light_colour
	vec4 light_colour = (intensity / attenuation) * spot.light_colour;

	// Phong shading
	vec4 diffuse = (mat.diffuse_reflection * light_colour) * max(dot(normal, light_dir), 0.0);
	vec3 half_vector = normalize(light_dir + view_dir);
	vec4 specular = (mat.specular_reflection * light_colour) * pow(max(dot(normal, half_vector), 0.0), mat.shininess);
	vec4 colour = ((mat.emissive + diffuse) * tex_colour) + specular;
	colour.a = 1.0;

	return colour;
}

void main() {
colour = vec4(0.0, 0.0, 0.0, 1.0);
  // Calculate view direction
	vec3 view_dir = normalize(eye_pos - position);
  // Sample texture
	vec4 tex_colour = texture(tex, tex_coord);
  // Calculate directional light colour
	//colour += calculate_direction(light, mat, normal, view_dir, tex_colour);
	
  // Sum point lights
	for (int i = 0; i < points.length(); i++)
	{
		colour += calculate_point(points[i], mat, position, normal, view_dir, tex_colour);
	}

  // Sum spot lights
	for (int i = 0; i < spots.length(); i++)
	{
		colour += calculate_spot(spots[i], mat, position, normal, view_dir, tex_colour);
	}
	
   colour.a = 1.0;
}