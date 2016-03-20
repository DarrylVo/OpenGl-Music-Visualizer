#version 330 core

in vec4 outColor;
in vec2 UV;
out vec4 color;


uniform sampler2D myTextureSampler;

void main()
{
		color = texture( myTextureSampler, UV );
	
		color *= outColor;	
		
}


