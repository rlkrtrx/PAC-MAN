#version 330 core

layout (location=0) in vec2 pos;
layout (location=1) in vec2 texPos;

out vec4 tPos;

uniform mat4 ortho;
uniform mat4 model;
uniform mat4 texModel;

void main()
{
  vec4 final = ortho * model * vec4(pos, 0.0f, 1.0f);
	gl_Position = final;

        tPos = texModel * vec4(texPos, 0.0f, 1.0f);
}
