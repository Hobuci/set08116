#version 440

// Main textures
uniform sampler2D tex[2];
// Blend map
uniform sampler2D blend;

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;
// Outgoing fragment colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Sample the two main textures
  vec4 c1 = texture(tex[0], tex_coord);
  vec4 c2 = texture(tex[1], tex_coord);

  // Sample the blend texture
  vec4 blendColor = texture(blend, tex_coord);

  // Mix the main samples using r component from blend value
    colour = mix(c1, c2, blendColor);
  // *********************************
}