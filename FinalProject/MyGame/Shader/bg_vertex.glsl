#version 330 core

layout (location = 0) in vec3 inPos;		// 도형 좌표값
layout (location = 1) in vec3 inTexcoord;	// 점의 텍스처 좌표 저장했던걸
layout (location = 2) in vec3 inNormal;		// 점의 법선벡터

out vec3 outTexcoord; // 텍스처 좌표 OUT 시켜서 fragment.glsl에서 읽을것임
out vec3 outFragPos;	// .
out vec3 outNormal;		// . 마찬가지로..


uniform mat4 modelTransform;


void main()
{
	outTexcoord = inTexcoord;  // 텍스처 좌표 out 시켜서 fragment.glsl에서 읽을것임
	outFragPos = vec3(modelTransform * vec4(inPos, 1.f));
	outNormal = mat3(modelTransform) * inNormal;	// x, y, z축에 대하여 같은 크기의 (변환을했다면..필요하면..)

	gl_Position =  vec4(outFragPos, 1.f);
}
