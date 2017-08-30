#version 430

out vec4 oFragColor;

vec3 MPRCasting();

void main()
{
    oFragColor = vec4(MPRCasting(), 1.0);
}