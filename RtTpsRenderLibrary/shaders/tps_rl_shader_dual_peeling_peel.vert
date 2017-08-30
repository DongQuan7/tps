varying vec4 vTexCoord;

void ShadeVertex();

void main(void)
{
    gl_Position = ftransform();
    vTexCoord = gl_MultiTexCoord0;
    ShadeVertex();
}