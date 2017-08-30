#version 430
in vec4 vVertex;
in vec4 vTexCoord;
//uniform mat4 matMVP;
    
out vec4 oTexCoord;
void main() {
    //gl_Position = matMVP * vVertex;
    gl_Position = vVertex;
    oTexCoord = vTexCoord;
}