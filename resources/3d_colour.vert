#version 120

attribute vec3 in_Position;

uniform vec4 in_Colour;
uniform mat4 MVP;

varying vec4 pass_Colour;

void main() 
{
	pass_Colour = in_Colour;
	gl_Position = MVP * vec4(in_Position,1.0);
}
