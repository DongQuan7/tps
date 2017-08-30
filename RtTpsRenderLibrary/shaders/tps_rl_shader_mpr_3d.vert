void CalcMprCastingPoints(vec4 vertex);

void ShadeVertex()
{
    CalcMprCastingPoints(gl_Vertex);
}