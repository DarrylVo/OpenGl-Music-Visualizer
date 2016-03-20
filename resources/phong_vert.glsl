#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPos;

out vec3 fragNor;
out vec3 lightV;
out vec3 viewV;
out vec2 text;
out vec4 repeat;


void main()
{
	
	text = vertTex;
	gl_Position = P * V*M * vertPos;
	repeat = gl_Position;
	fragNor =  normalize((V*M * vec4(vertNor, 0.0)).xyz);
	lightV = normalize(lightPos-vec3(V*M*vertPos));
	lightV = normalize(vec3(0.2,5,-2));
	viewV = normalize(vec3(-(V*M*vertPos)));	
}