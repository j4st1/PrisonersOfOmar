#pragma once
#include <GL/glew.h>

class ShadowMap {
public:
    explicit ShadowMap(int size = 2048);
    ~ShadowMap();

    ShadowMap(const ShadowMap&) = delete;
    ShadowMap& operator=(const ShadowMap&) = delete;

    void bindForWriting() const;
    void unbind(int screenWidth, int screenHeight) const;

    GLuint depthTexture() const { return m_depth; }
    int    size()         const { return m_size; }

private:
    GLuint m_fbo   = 0;
    GLuint m_depth = 0;
    int    m_size  = 0;
};
