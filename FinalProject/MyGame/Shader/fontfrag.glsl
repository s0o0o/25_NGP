#version 330 core

in vec2 v_UV;
out vec4 FragColor;

uniform sampler2D fontAtlas;
uniform vec4 uvRect; // (uMin, vMin, uWidth, vHeight)

void main()
{
    vec2 atlasUV = uvRect.xy + v_UV * uvRect.zw;
    vec4 texColor = texture(fontAtlas, atlasUV);
    float alpha = texColor.a;

    FragColor = texColor;
}

