#version 330 core

in vec3 outColor;
in vec3 outFragPos;
in vec3 outNormal;

out vec4 FragColor;

//uniform vec3 cameraPos;

const float ambientLight = 1.0f;
uniform vec3 lightColor = vec3(1.f, 0.88f, 1.f);	

uniform vec3 lightPos; 

void main()
{
	vec3 ambient = ambientLight * lightColor;

	vec3 vNormal = normalize(outNormal);
	vec3 lightDir = normalize(lightPos - outFragPos);		// 빛의 방향벡터
	float diffuseLight = max(dot(vNormal, lightDir), 0.0);	// 0 ~ 1사이의 값
	vec3 diffuse = diffuseLight * lightColor;
	
//	int shininess = 128;
//	vec3 cameraDir = normalize(cameraPos - outFragPos);
//	vec3 reflectDir = reflect(-lightDir, vNormal);
//	float specularLight = max(dot(cameraDir, reflectDir), 0.0);	// 0 ~ 1사이의 값
//	specularLight = pow(specularLight, shininess);
//	vec3 specular = specularLight * lightColor;

	vec3 result = (ambient + diffuse /*+ specular*/) * outColor;		// vec3 * vec3 = 원소끼리의 곱
	
	FragColor = vec4(result, 1.0);
}
