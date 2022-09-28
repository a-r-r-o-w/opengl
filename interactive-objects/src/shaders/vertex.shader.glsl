#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main () {
  gl_Position = u_projection * u_view * u_model * vec4(pos.xyz, 1.0);
  // gl_Position = vec4(pos.xyz, 1.0);
}
