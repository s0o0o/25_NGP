#version 330 core

in vec3 outTexcoord; // �ؽ�ó ��ġ���о�ð�..
in vec3 outFragPos;
in vec3 outNormal;

out vec4 FragColor;

uniform sampler2D outSampler;

uniform vec3 cameraPos;



void main()
{
	// x == r == s, y == g == t,
	vec4 outColor = texture(outSampler, outTexcoord.st);

	vec3 result = outColor.rgb;		// vec3 * vec3 = ���ҳ����� ��
	
	FragColor = vec4(result, 1.0);
}
