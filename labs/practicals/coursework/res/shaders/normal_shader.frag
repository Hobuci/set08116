#version 440

// Directional light structure
#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT
struct directional_light {
  vec4 ambient_intensity;
  vec4 light_colour;
  vec3 light_dir;
};
#endif

// Point light information
#ifndef POINT_LIGHT
#define POINT_LIGHT
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};
#endif

// Spot light data
#ifndef SPOT_LIGHT
#define SPOT_LIGHT
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};
#endif

// A material structure
#ifndef MATERIAL
#define MATERIAL
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};
#endif

// Forward declarations of functions
vec4 calculate_direction(in directional_light light, in material mat, in vec3 normal, in vec3 view_dir, in vec4 tex_colour);
vec4 calculate_point(in point_light points, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour);
vec4 calculate_spot(in spot_light spots, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour);
vec3 calculate_normal(in vec3 normal, in vec3 tangent, in vec3 binormal, in sampler2D normal_map, in vec2 tex_coord);
float calculate_shadow(in sampler2D shadow_map, in vec4 light_space_pos);

// Directional light information
uniform directional_light light;
// Point lights being used in the scene
uniform point_light points[7];
// Spot lights being used in the scene
uniform spot_light spots[5];
// Material of the object being rendered
uniform material mat;
// Position of the eye
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;
// Normal map to sample from
uniform sampler2D normal_map;
// Shadow map to sample from
uniform sampler2D shadow_map;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming texture coordinate
layout(location = 1) in vec2 tex_coord;
// Incoming normal
layout(location = 2) in vec3 normal;
// Incoming tangent
layout(location = 3) in vec3 tangent;
// Incoming binormal
layout(location = 4) in vec3 binormal;
// Incoming light space position
layout(location = 5) in vec4 light_space_pos;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
colour = vec4(0.0, 0.0, 0.0, 1.0);
	// Calculate view direction
	vec3 view_dir = normalize(eye_pos - position);
	// Sample texture
	vec4 tex_colour = texture(tex, tex_coord);
	// Calculate shade factor
	//float shade = calculate_shadow(shadow_map, light_space_pos);
	
	// Calculate normal from normal map
	vec3 normal_map_normal = calculate_normal(normal, tangent, binormal, normal_map, tex_coord);
	
	// Calculate directional light colour
	//colour += calculate_direction(light, mat, normal_map_normal, view_dir, tex_colour);
	
	// Sum point lights
	for (int i = 0; i < points.length(); i++)
	{
		colour += calculate_point(points[i], mat, position, normal_map_normal, view_dir, tex_colour);
	}

	// Sum spot lights
	for (int i = 0; i < spots.length(); i++)
	{
		colour += calculate_spot(spots[i], mat, position, normal_map_normal, view_dir, tex_colour);
	}
	
	// Scale colour by shade
    //colour *= shade;
	
	colour.a = 1.0;
}