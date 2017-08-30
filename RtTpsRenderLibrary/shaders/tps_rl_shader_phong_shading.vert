varying vec3 N;
varying vec3 v;

void ShadeVertex()
{
    v = vec3(gl_ModelViewMatrix * gl_Vertex);
    N = normalize(gl_NormalMatrix * gl_Normal);
}
