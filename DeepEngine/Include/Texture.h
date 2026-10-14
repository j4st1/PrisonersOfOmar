#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class Texture {
public:
    Texture() = default;
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& o) noexcept;
    Texture& operator=(Texture&& o) noexcept;

    // ---- procedural factories ----
    static Texture checker(int size, int cells, glm::u8vec3 a, glm::u8vec3 b);
    static Texture noise  (int size, unsigned seed, glm::u8vec3 base, int variance);
    static Texture grass  (int size, unsigned seed);
    static Texture brick  (int size, glm::u8vec3 mortar, glm::u8vec3 brick);
    static Texture wood   (int size, glm::u8vec3 base);
    static Texture solid  (int size, glm::u8vec3 color);

    void bind(int unit) const;
    GLuint id() const { return m_id; }

private:
    void upload(const unsigned char* data, int size);
    void release();

    GLuint m_id = 0;
};
