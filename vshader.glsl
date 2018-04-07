#version 130

in vec4 vPosition;
in vec3 vColor;
out vec4 color;

uniform int win_x;
uniform int win_y;

void main() 
{
	// default is normalized by 1 therefore, total length of x and y == 2
	// divide by the size of the window to make it fit
	mat4 scale = mat4(2.0/win_x,	0.0,		0.0,	0.0,
			  0.0,		2.0/win_y,	0.0,	0.0,
			  0.0,		0.0,		1.0,	0.0,
			  0.0,		0.0,		0.0,	1.0);
	
	// translation for centering: (0, 0) to (400, 700) => (-200, -350) to (200, 350) 
	vec4 newPosition = vPosition + vec4(-200, -350, 0, 0);
	
	// then apply the scaling
	gl_Position = scale * newPosition; 

	color = vec4(vColor, 1.0);	
} 


