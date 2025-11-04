#version 330 core

layout (location = 0) in vec2 in_Pos;
layout (location = 1) in vec2 in_UV;

out vec2 v_UV;

uniform mat4 model;         // 글자 위치/크기 (2D)
uniform mat4 projection;    // 2D 화면용 Ortho 매트릭스

void main()
{
    gl_Position = projection * model * vec4(in_Pos, 0.0, 1.0);
    v_UV = in_UV;
}