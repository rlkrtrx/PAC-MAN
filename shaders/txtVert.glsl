#version 330 core

layout (location=0) in vec2 pos;
layout (location=1) in vec2 in_tex_pos;

uniform mat4 model;
uniform mat4 proj;

out vec2 tex_pos;

void main()
{
  gl_Position = proj * model * vec4(pos, 0.0f, 1.0f);
  tex_pos = in_tex_pos;
}
