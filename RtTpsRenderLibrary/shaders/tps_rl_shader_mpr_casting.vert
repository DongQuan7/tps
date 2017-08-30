#version 430

uniform mat4 matMPRModel2VolumeModel;
uniform vec4 vCastingVector;

out vec4 vStart;
out vec4 vEnd;

void CalcMprCastingPoints(vec4 vertex)
{
    vStart= matMPRModel2VolumeModel * (vertex + vCastingVector * 0.5);
    vEnd = matMPRModel2VolumeModel * (vertex - vCastingVector * 0.5);
}