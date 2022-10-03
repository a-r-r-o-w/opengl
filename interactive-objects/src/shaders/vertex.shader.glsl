#version 330 core

layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec3 i_color;

out vec3 color;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main () {
  gl_Position = u_projection * u_view * u_model * vec4(i_pos.xyz, 1.0);
  color = i_color;
}
