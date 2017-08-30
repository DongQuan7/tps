#include "StdAfx.h"

#include <stdio.h>
#include "RtTpsRenderLibrary/tps_rl_gl_program.h"
#include "gl/glew.h"

TPS_BEGIN_NAMESPACE

GLProgram::GLProgram() : is_valid(false)
{
    p_id = glCreateProgram();
}

GLProgram::~GLProgram()
{
    __Clear();
}

void GLProgram::__Clear()
{
    for (size_t i = 0; i < v_ids.size(); i++) {
        glDeleteShader(v_ids[i]);
    }
    for (size_t i = 0; i < f_ids.size(); i++) {
        glDeleteShader(f_ids[i]);
    }
    for (size_t i = 0; i < g_ids.size(); i++)
    {
        glDeleteShader(g_ids[i]);
    }
    for (size_t i = 0; i < c_ids.size(); i++)
    {
        glDeleteShader(c_ids[i]);
    }
    if (p_id != 0) {
        glDeleteProgram(p_id);
    }
    for (size_t i = 0 ; i < v_shaders.size(); ++i)
    {
        TPS_DEL_ARRAY(v_shaders[i]);
    }
    v_shaders.clear();
    for (size_t i = 0 ; i < f_shaders.size(); ++i)
    {
        TPS_DEL_ARRAY(f_shaders[i]);
    }
    f_shaders.clear();
    for (size_t i = 0 ; i < g_shaders.size(); ++i)
    {
        TPS_DEL_ARRAY(g_shaders[i]);
    }
    g_shaders.clear();
    for (size_t i = 0 ; i < c_shaders.size(); ++i)
    {
        TPS_DEL_ARRAY(c_shaders[i]);
    }
    c_shaders.clear();
    is_valid = false;
}

void GLProgram::Bind()
{
    if (!is_valid)
    {
        throw std::exception("GLProgram object has not been initialized!");
    }

    glUseProgram(p_id);
}

void GLProgram::Unbind()
{
    glUseProgram(0);
}

bool GLProgram::Link()
{
    if (is_valid)
    {
        printf("\nThe program has already been linked.");
        return true;
    }

    glGetError();
    GLint status = 0;
    for (size_t i = 0 ; i < v_shaders.size(); ++i)
    {
        GLuint id = glCreateShader(GL_VERTEX_SHADER);
        v_ids.push_back(id);
        glShaderSource(id, 1, &v_shaders[i], nullptr);
        if (GL_NO_ERROR != glGetError()){
            printf("\nFailed to load vertex shader.");
            //LOG
            return false;
        }
        glCompileShader(id);
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE){
            char infoLog[1024];
            glGetShaderInfoLog(id, 1024, nullptr, infoLog);
            printf("\nFailed to compile vertex shader, error info: \n%s", infoLog);
            //LOG
            return false;
        }
        delete[] v_shaders[i];
    }
    v_shaders.clear();

    for (size_t i = 0 ; i < f_shaders.size(); ++i)
    {
        GLuint id = glCreateShader(GL_FRAGMENT_SHADER);
        f_ids.push_back(id);
        glShaderSource(id, 1, &f_shaders[i], nullptr);
        if (GL_NO_ERROR != glGetError()){
            printf("\nFailed to load fragment shader.");
            //LOG
            return false;
        }
        glCompileShader(id);
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE){
            char infoLog[1024];
            glGetShaderInfoLog(id, 1024, nullptr, infoLog);
            printf("\nFailed to compile fragment shader, error info: \n%s", infoLog);
            //LOG
            return false;
        }
        delete[] f_shaders[i];
    }
    f_shaders.clear();

    for (size_t i = 0 ; i < g_shaders.size(); ++i)
    {
        GLuint id = glCreateShader(GL_GEOMETRY_SHADER);
        g_ids.push_back(id);
        glShaderSource(id, 1, &g_shaders[i], nullptr);
        if (GL_NO_ERROR != glGetError()){
            printf("\nFailed to load geometry shader.");
            //LOG
            return false;
        }
        glCompileShader(id);
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE){
            char infoLog[1024];
            glGetShaderInfoLog(id, 1024, nullptr, infoLog);
            printf("\nFailed to compile geometry shader, error info: \n%s", infoLog);
            //LOG
            return false;
        }
        delete[] g_shaders[i];
    }
    g_shaders.clear();

    for (size_t i = 0 ; i < c_shaders.size(); ++i)
    {
        GLuint id = glCreateShader(GL_COMPUTE_SHADER);
        c_ids.push_back(id);
        glShaderSource(id, 1, &c_shaders[i], nullptr);
        if (GL_NO_ERROR != glGetError()){
            printf("\nFailed to load compute shader.");
            //LOG
            return false;
        }
        glCompileShader(id);
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE){
            char infoLog[1024];
            glGetShaderInfoLog(id, 1024, nullptr, infoLog);
            printf("\nFailed to compile compute shader, error info: \n%s", infoLog);
            //LOG
            return false;
        }
        delete[] c_shaders[i];
    }
    c_shaders.clear();

    for (size_t i = 0; i < v_ids.size(); ++i)
    {
        glAttachShader(p_id, v_ids[i]);
    }
    for (size_t i = 0; i < g_ids.size(); ++i)
    {
        glAttachShader(p_id, g_ids[i]);
    }
    for (size_t i = 0; i < f_ids.size(); ++i)
    {
        glAttachShader(p_id, f_ids[i]);
    }
    for (size_t i = 0; i < c_ids.size(); ++i)
    {
        glAttachShader(p_id, c_ids[i]);
    }

    if (!g_ids.empty())
    {
        glProgramParameteri(p_id, GL_GEOMETRY_INPUT_TYPE_ARB , g_input_type);
        glProgramParameteri(p_id, GL_GEOMETRY_OUTPUT_TYPE_ARB , g_output_type);
        glProgramParameteri(p_id, GL_GEOMETRY_VERTICES_OUT_ARB, g_vertices_out);
    }

    glLinkProgram(p_id);
    glGetProgramiv(p_id, GL_LINK_STATUS, &status);
    if (status != GL_TRUE){
        char infoLog[1024];
        glGetProgramInfoLog(p_id, 1024, nullptr, infoLog);
        printf("\nFailed to link program: error info: \n%s", infoLog);
        return false;
    }
    is_valid = true;
    return true;
}

void GLProgram::AttachShader(GLProgram::ShaderType type, const char* shader_str)
{
    if (shader_str == nullptr) return;
    char* shaderStr = new char[strlen(shader_str) + 1];
    strcpy_s(shaderStr, strlen(shader_str) + 1, shader_str);
    switch(type)
    {
    case GLProgram::Vertex_Shader:
        v_shaders.push_back(shaderStr);
        break;
    case GLProgram::Geometry_Shader:
        g_shaders.push_back(shaderStr);
        break;
    case GLProgram::Fragment_Shader:
        f_shaders.push_back(shaderStr);
        break;
    case GLProgram::Compute_Shader:
        c_shaders.push_back(shaderStr);
        break;
    default:
        printf("Unsupported shader type.");
        delete []shader_str;
        return;
    }
}

void GLProgram::SetGeometryShaderParameters(GLenum input_type, GLenum output_type, GLuint vertices_out)
{
    g_input_type = input_type;
    g_output_type = output_type;
    g_vertices_out =vertices_out;
}

bool GLProgram::SetTextureUnit(std::string tex_name, int texunit)
{
    if (!is_valid)
    {
        throw std::exception("GLProgram object has not been initialized yet!");
    }

    GLint id = glGetUniformLocation(p_id, tex_name.c_str());
    if (id == -1) 
    {
        //log
        return false;
    }
    glUniform1i(id, texunit);
    return true;
}

bool GLProgram::BindTexture(int tex_unit, GLenum target, GLuint tex_id, const std::string& tex_name)
{
    if (!is_valid)
    {
        throw std::exception("GLProgram object has not been initialized yet!");
    }

    glActiveTexture(GL_TEXTURE0 + tex_unit);
    glBindTexture(target, tex_id);
    bool ret = SetTextureUnit(tex_name, tex_unit);
    glActiveTexture(GL_TEXTURE0);
    return ret;
}

bool GLProgram::SetUniformf(std::string name, GLfloat* val, int count)
{
    if (!is_valid)
    {
        throw std::exception("GLProgram object has not been initialized yet!");
    }

    GLint id = glGetUniformLocation(p_id, name.c_str());
    if (id == -1) {
        //log uniform does not exist
        return false;
    }

    switch (count) {
    case 1:
        glUniform1fv(id, 1, val);
        break;
    case 2:
        glUniform2fv(id, 1, val);
        break;
    case 3:
        glUniform3fv(id, 1, val);
        break;
    case 4:
        glUniform4fv(id, 1, val);
        break;
    case 16:
        glUniformMatrix4fv(id, 1, GL_FALSE, val);
        break;
    }
    return true;
}

bool GLProgram::SetUniformi(std::string name, GLint* val, int count)
{
    if (!is_valid)
    {
        throw std::exception("GLProgram object has not been initialized yet!");
    }

    GLint id = glGetUniformLocation(p_id, name.c_str());
    if (id == -1) {
        //log uniform does not exist
        return false;
    }

    switch (count) {
    case 1:
        glUniform1iv(id, 1, val);
        break;
    case 2:
        glUniform2iv(id, 1, val);
        break;
    case 3:
        glUniform3iv(id, 1, val);
        break;
    case 4:
        glUniform4iv(id, 1, val);
        break;
    }
    return true;
}

bool GLProgram::SetUniformui(std::string name, GLuint* val, int count)
{
    if (!is_valid)
    {
        throw std::exception("GLProgram object has not been initialized yet!");
    }

    GLint id = glGetUniformLocation(p_id, name.c_str());
    if (id == -1) {
        //log uniform does not exist
        return false;
    }

    switch (count) {
    case 1:
        glUniform1uiv(id, 1, val);
        break;
    case 2:
        glUniform2uiv(id, 1, val);
        break;
    case 3:
        glUniform3uiv(id, 1, val);
        break;
    case 4:
        glUniform4uiv(id, 1, val);
        break;
    }
    return true;
}

TPS_END_NAMESPACE
