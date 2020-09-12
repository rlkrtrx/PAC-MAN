#version 330 core

layout (location=0) in vec2 pos;
layout (location=1) in vec2 tpos;
layout (location=2) in mat4 mdl;
layout (location=7) in mat4 tmdl;

uniform mat4 prj;

out vec4 tPos;

void main()
{
	gl_Position = prj * mdl * vec4(pos, 0.0f, 1.0f);
        tPos = tmdl * vec4(tpos, 0.0f, 1.0f);
}
