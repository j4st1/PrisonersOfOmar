#include <Shader.h>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <vector>
#include <iostream>

static GLuint compile(GLenum type, const char* src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len > 0 ? len : 1);
        glGetShaderInfoLog(s, len, nullptr, log.data());
        std::string msg = "Shader compile error: ";
        msg += log.data();
        glDeleteShader(s);
        throw std::runtime_error(msg);
    }
    return s;
}

Shader::Shader(const char* vertexSrc, const char* fragmentSrc)
{
    GLuint vs = compile(GL_VERTEX_SHADER, vertexSrc);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fragmentSrc);

    m_id = glCreateProgram();
    glAttachShader(m_id, vs);
    glAttachShader(m_id, fs);
    glLinkProgram(m_id);

    GLint ok = 0;
    glGetProgramiv(m_id, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(len > 0 ? len : 1);
        glGetProgramInfoLog(m_id, len, nullptr, log.data());
        std::string msg = "Shader link error: ";
        msg += log.data();
        glDeleteShader(vs);
        glDeleteShader(fs);
        glDeleteProgram(m_id);
        m_id = 0;
        throw std::runtime_error(msg);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::~Shader()
{
    release();
}

Shader::Shader(Shader&& o) noexcept : m_id(o.m_id)
{
    o.m_id = 0;
}

Shader& Shader::operator=(Shader&& o) noexcept
{
    if (this != &o) {
        release();
        m_id = o.m_id;
        o.m_id = 0;
    }
    return *this;
}

void Shader::release()
{
    if (m_id) {
        glDeleteProgram(m_id);
        m_id = 0;
    }
}

void Shader::use() const
{
    glUseProgram(m_id);
}

void Shader::setInt(const char* name, int v) const
{
    glUniform1i(glGetUniformLocation(m_id, name), v);
}

void Shader::setFloat(const char* name, float v) const
{
    glUniform1f(glGetUniformLocation(m_id, name), v);
}

void Shader::setVec3(const char* name, const glm::vec3& v) const
{
    glUniform3fv(glGetUniformLocation(m_id, name), 1, glm::value_ptr(v));
}

void Shader::setMat3(const char* name, const glm::mat3& m) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::setMat4(const char* name, const glm::mat4& m) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, glm::value_ptr(m));
}
