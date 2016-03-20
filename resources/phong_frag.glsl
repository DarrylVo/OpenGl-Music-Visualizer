#version 330 core 
in vec3 fragNor;
in vec3 lightV;
in vec3 viewV;
in vec2 text;
in vec4 repeat;

out vec4 color;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform float MatShine;
uniform vec3 MatSpec;
//uniform vec3 lightCol;

uniform float flag_nc;
uniform mat4 M;
uniform mat4 V;
uniform sampler2D ti;

uniform  vec3 colorC;

vec3 H;
vec4 pos;

float s_scalar;
float d_scalar;







void main()
{
	
	
	
	
	d_scalar = max(dot(normalize(lightV),normalize(fragNor)),0);
	
	H = (normalize(viewV)+normalize(lightV))/2;
	s_scalar = pow(max( dot(normalize(H),normalize(fragNor)),0),MatShine);
	
	color = vec4((MatDif*d_scalar) + MatAmb + (MatSpec*s_scalar),1);
	vec2 c = vec2(0,0);
	
	c.x = repeat.x+repeat.z;
	c.y = repeat.y+repeat.x;
	if(flag_nc == 1.0) {
		//color*=texture2D(ti,text);
		color = vec4(0,1.0,0,1);
	}
	color = vec4(colorC,1);
	
	
	
}