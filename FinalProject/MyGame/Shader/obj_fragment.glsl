#version 330 core

in vec3 outColor;
in vec3 outFragPos;
in vec3 outNormal;

out vec4 FragColor;

uniform vec3 cameraPos;

const float ambientLight = 0.5f;
uniform vec3 lightColor;
const vec3 lightPos = vec3(5.f, 40.f, 0.f);

uniform bool useLight;

void main()
{
	vec3 ambient = ambientLight * lightColor;

	vec3 vNormal = normalize(outNormal);
	vec3 lightDir = normalize(lightPos - outFragPos);		// ���� ���⺤��
	float diffuseLight = max(dot(vNormal, lightDir), 0.0);	// 0 ~ 1������ ��
	vec3 diffuse = diffuseLight * lightColor;
	
	int shininess = 64;
	vec3 cameraDir = normalize(cameraPos - outFragPos);
	vec3 reflectDir = reflect(-lightDir, vNormal);
	float specularLight = max(dot(cameraDir, reflectDir), 0.0);	// 0 ~ 1������ ��
	specularLight = pow(specularLight, shininess);
	vec3 specular = specularLight * lightColor;

	vec3 result = (ambient + diffuse + specular) * outColor;		// vec3 * vec3 = ���ҳ����� ��
	
	if(useLight){
		FragColor = vec4(result, 1.0);
	}
	else{
		FragColor = vec4(outColor, 1.0);
	}
}
