#version 330 core

in vec3 color;

uniform bool u_use_vertex_color;
uniform float u_blend;
uniform vec4 u_color;

out vec4 FragColor;

void main () {
  if (u_use_vertex_color)
    FragColor = vec4(color.xyz, u_blend);
  else
    FragColor = u_color;
}
