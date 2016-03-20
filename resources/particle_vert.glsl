#version 330 core
layout(location = 0) in vec3 billboard;
layout(location = 1) in vec4 pos;
layout(location = 2) in vec4 color;
uniform mat4 P;
uniform mat4 V;

uniform vec3 Up;
uniform vec3 Right;

out vec4 outColor;
out vec2 UV;

void main()
{
	outColor = color;
	vec3 particleCenter = pos.xyz;
	
	vec3 vertexPosition = particleCenter + (Right *billboard.x)+(Up*billboard.y);
	gl_Position = P*V*vec4(vertexPosition,1);
	
	UV = billboard.xy+vec2(0.5,0.5);
		
	
	
	
}
