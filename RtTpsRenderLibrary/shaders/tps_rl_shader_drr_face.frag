#version 430

in vec4 vVertexColor;
out vec4 vFragColor;

void main()
{
   vFragColor = vVertexColor;
}