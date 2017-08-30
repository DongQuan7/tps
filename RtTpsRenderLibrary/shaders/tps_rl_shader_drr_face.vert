#version 430

uniform mat4 matMVP;
in vec4 vVertex;
in vec4 vColor;
out vec4 vVertexColor;

void main()
{
   vVertexColor = vColor;
   gl_Position = matMVP * vVertex;
}