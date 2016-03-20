#version 330 core 


in vec3 g_color;
out vec4 color;
in vec3 fragNor;

uniform float flag_nc;

void main()
{
	if(flag_nc == 1.0) {
		color = vec4(fragNor,1.0);
	}
	else {
	
	
		color = vec4(g_color,1.0);
	}
}
