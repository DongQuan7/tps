uniform sampler2DRect Tex;

void main(void)
{
    gl_FragColor = texture(Tex, gl_FragCoord.xy);
    if (gl_FragColor.a == 0) discard;
}
