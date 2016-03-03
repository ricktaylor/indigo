#version 120

uniform sampler2D texture0;

varying vec4 pass_Colour;
varying vec2 pass_TexCoord;

void main() 
{
	gl_FragColor = pass_Colour * texture2D(texture0,pass_TexCoord);
}
