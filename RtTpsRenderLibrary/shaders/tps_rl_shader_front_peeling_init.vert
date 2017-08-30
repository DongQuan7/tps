void ShadeVertex();

void main(void)
{
    gl_Position = ftransform();
    ShadeVertex();
}