uniform float fAlpha;

vec3 MPRCasting();

vec4 ShadeFragment()
{
    return vec4(MPRCasting(), fAlpha);
}