uniform sampler2DRect tex;

void main(void)
{
    gl_FragColor = vec4(texture(tex, gl_FragCoord.xy).xyz, 1.0);
}