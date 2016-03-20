#version 330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform float flag_nc;

out vec3 fragNor;

uniform vec3 lightPos;

vec3 lightV;
vec3 viewV;



//uniform vec3 lightCol;
vec3 H;

float s_scalar;
float d_scalar;

out vec3 g_color;



uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float MatShine;

void main()
{
	
	
	gl_Position = P * V*M*  vertPos;
	fragNor = (M * vec4(vertNor, 0.0)).xyz;
	viewV = normalize(vec3(-(M*vertPos)));	
	
	lightV = normalize(lightPos-vec3(M*vertPos));
	
	d_scalar = max(dot(lightV,normalize(fragNor)),0);
	
	H = (viewV+lightV)/2;
	s_scalar = pow(max( dot(H,normalize(fragNor)),0),MatShine);
	

	g_color = d_scalar*MatDif + MatAmb + (s_scalar*MatSpec);
	
	
}
