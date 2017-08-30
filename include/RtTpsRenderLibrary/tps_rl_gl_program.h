#ifndef GL_PROGRAM_H_
#define GL_PROGRAM_H_

#include "GL\glew.h"
#include <vector>
#include <string>

#include "tps_rl_defs.h"

TPS_BEGIN_NAMESPACE

class TPS_RL_DECLSPEC GLProgram
{
public:
    enum ShaderType
    {
        Vertex_Shader,
        Tessellation_Control_Shader,
        Tessellation_Evaluation_Shader,
        Geometry_Shader,
        Fragment_Shader,
        Compute_Shader,
    };

    GLProgram();
    virtual ~GLProgram();

    void Bind();
    void Unbind();
    bool Link();
    void AttachShader(ShaderType type, const char* shader_str);
    void SetGeometryShaderParameters(GLenum input_type, GLenum output_type, GLuint vertices_out);

    inline GLuint ID(){ return p_id; }

    bool BindTexture(int tex_unit, GLenum target, GLuint tex_id, const std::string& tex_name);
    bool SetTextureUnit(std::string texname, int texunit);
    bool SetUniformf(std::string name, GLfloat* val, int count);
    bool SetUniformi(std::string name, GLint* val, int count);
    bool SetUniformui(std::string name, GLuint* val, int count);
private:
    void __Clear();

    bool is_valid;

    GLuint p_id;
    std::vector<GLuint> v_ids;
    std::vector<GLuint> f_ids;
    std::vector<GLuint> g_ids;
    std::vector<GLuint> c_ids;

    std::vector<const char*> v_shaders;
    std::vector<const char*> f_shaders;
    std::vector<const char*> g_shaders;
    std::vector<const char*> c_shaders;

    GLenum g_input_type;
    GLenum g_output_type;
    GLuint g_vertices_out;

    TPS_DISALLOW_COPY_AND_ASSIGN(GLProgram);
};

TPS_END_NAMESPACE
// end namespace ***

#endif
