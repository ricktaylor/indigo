#version 120

attribute vec2 in_Position;
attribute vec2 in_TexCoord;

uniform vec4 in_Colour;
uniform mat4 MVP;

varying vec4 pass_Colour;
varying vec2 pass_TexCoord;

void main() 
{
	pass_Colour = in_Colour;
	pass_TexCoord = in_TexCoord;
	gl_Position = MVP * vec4(in_Position,0.0,1.0);
}
