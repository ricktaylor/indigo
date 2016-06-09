#version 120

uniform sampler2D texture0;

varying vec4 pass_Colour;
varying vec2 pass_TexCoord;

void main() 
{
	gl_FragColor = (texture2D(texture0,pass_TexCoord) * pass_Colour.a) + (pass_Colour * (1.0 - pass_Colour.a));
}
