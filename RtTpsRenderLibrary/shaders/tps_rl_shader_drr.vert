#version 430

in vec2 vInTexCoord;
in vec4 vVertex;
out vec2 vTexCoord;

void main()
{
   vTexCoord = vInTexCoord;
   gl_Position = vVertex;
}