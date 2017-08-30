varying vec3 N;
varying vec3 v;

uniform vec3 vLightColor;
uniform vec3 vLightPosition;
uniform vec4 vLightFactor;

vec4 CalculateFragmentColor();

vec4 ShadeFragment()
{
    vec3 L = normalize(vLightPosition - v);
    vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)
    vec3 R = normalize(-reflect(L,N));

    //material color
    vec4 color = CalculateFragmentColor();

    //calculate Ambient Term:
    vec3 amb = color.xyz * vLightColor * vLightFactor.x;

    //calculate Diffuse Term:
    vec3 diff = color.xyz * vLightColor * vLightFactor.y * max(dot(N,L), 0.0);
   
    // calculate Specular Term:
    vec3 spec = color.xyz * vLightColor * vLightFactor.z
        * pow(max(dot(R,E),0.0), 0.3 * vLightFactor.w);

    // write Total Color:
    vec4 total = vec4(amb + diff + spec, color.a);
    total = clamp(total, 0.0, 1.0);
    return total;
}