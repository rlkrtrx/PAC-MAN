#version 330 core

in vec4 tPos;

in vec2 vpos;

out vec4 color;

uniform sampler2D tex;

void main()
{
  color = texture(tex, vec2(tPos.xy));
}
