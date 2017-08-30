uniform sampler2DRect ColorTex;

void main(void)
{
    gl_FragColor = textureRect(ColorTex, gl_FragCoord.xy);
}