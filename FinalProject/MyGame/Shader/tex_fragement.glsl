#version 330 core

in vec3 outTexcoord; // 텍스처 위치들읽어올겨..
in vec3 outFragPos;
in vec3 outNormal;

out vec4 FragColor;

uniform sampler2D outSampler;
uniform vec3 cameraPos;
uniform vec3 lightColor;

uniform bool useLight;
uniform bool drawGrass;

void main()
{
	vec4 outColor = texture(outSampler, outTexcoord.st);
    vec3 result;
    
    if (drawGrass)
    {
        vec2 scaledUV = outTexcoord.st * 5.0f;
        outColor = texture(outSampler, scaledUV);
    }
    
    if (useLight)
    {
        result = outColor.rgb * lightColor;
    }
    else
    {
        result = outColor.rgb;
    }
   
    FragColor = vec4(result, outColor.a);
}
