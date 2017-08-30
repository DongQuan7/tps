#version 430

in vec4 inVertex;

//matMVP;
uniform mat4 matMVP;

void CalcMprCastingPoints(vec4 vertex);

void main()
{
    gl_Position = matMVP * inVertex;

    CalcMprCastingPoints(inVertex);
}