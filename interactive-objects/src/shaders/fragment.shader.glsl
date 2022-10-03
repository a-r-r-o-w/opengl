#version 330 core

in vec3 color;

uniform bool u_use_vertex_color;
uniform vec3 u_color;

out vec4 FragColor;

void main () {
  if (u_use_vertex_color)
    FragColor = vec4(color.xyz, 1.0f);
  else
    FragColor = vec4(u_color, 1.0f);
}
