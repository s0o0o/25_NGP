#version 330 core

layout (location = 0) in vec3 inPos;		// ���� ��ǥ��
layout (location = 1) in vec3 inTexcoord;	// ���� �ؽ�ó ��ǥ �����ߴ���
layout (location = 2) in vec3 inNormal;		// ���� ��������

out vec3 outTexcoord; // �ؽ�ó ��ǥ OUT ���Ѽ� fragment.glsl���� ��������
out vec3 outFragPos;	// .
out vec3 outNormal;		// . ����������..


uniform mat4 modelTransform;


void main()
{
	outTexcoord = inTexcoord;  // �ؽ�ó ��ǥ out ���Ѽ� fragment.glsl���� ��������
	outFragPos = vec3(modelTransform * vec4(inPos, 1.f));
	outNormal = mat3(modelTransform) * inNormal;	// x, y, z�࿡ ���Ͽ� ���� ũ���� (��ȯ���ߴٸ�..�ʿ��ϸ�..)

	gl_Position =  vec4(outFragPos, 1.f);
}
