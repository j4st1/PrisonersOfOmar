#include <Texture.h>
#include <vector>
#include <cmath>
#include <algorithm>

namespace {
    inline unsigned hashU(unsigned x) {
        x ^= x >> 16; x *= 0x7feb352dU;
        x ^= x >> 15; x *= 0x846ca68bU;
        x ^= x >> 16; return x;
    }
    inline float hashF(int x, int y, unsigned seed) {
        unsigned h = hashU(static_cast<unsigned>(x) * 374761393u +
                           static_cast<unsigned>(y) * 668265263u + seed);
        return (h & 0xFFFFFF) / static_cast<float>(0xFFFFFF);
    }
    inline float smoothstepf(float t) { return t * t * (3.0f - 2.0f * t); }

    float valueNoise(float x, float y, unsigned seed) {
        int xi = static_cast<int>(std::floor(x));
        int yi = static_cast<int>(std::floor(y));
        float xf = x - xi, yf = y - yi;
        float u = smoothstepf(xf), v = smoothstepf(yf);
        float a = hashF(xi,     yi,     seed);
        float b = hashF(xi + 1, yi,     seed);
        float c = hashF(xi,     yi + 1, seed);
        float d = hashF(xi + 1, yi + 1, seed);
        float ab = a + (b - a) * u;
        float cd = c + (d - c) * u;
        return ab + (cd - ab) * v;
    }
    float fbm(float x, float y, unsigned seed) {
        float v = 0.0f, a = 0.5f, f = 1.0f;
        for (int i = 0; i < 4; ++i) {
            v += a * valueNoise(x * f, y * f, seed + i * 7u);
            f *= 2.0f; a *= 0.5f;
        }
        return v;
    }
    unsigned char clamp255(int v) {
        return static_cast<unsigned char>(std::clamp(v, 0, 255));
    }
}

Texture::~Texture() { release(); }

Texture::Texture(Texture&& o) noexcept : m_id(o.m_id) { o.m_id = 0; }

Texture& Texture::operator=(Texture&& o) noexcept {
    if (this != &o) { release(); m_id = o.m_id; o.m_id = 0; }
    return *this;
}

void Texture::release() {
    if (m_id) { glDeleteTextures(1, &m_id); m_id = 0; }
}

void Texture::upload(const unsigned char* data, int size)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

// ------------------------------------------------------------------ solid

Texture Texture::solid(int size, glm::u8vec3 color)
{
    std::vector<unsigned char> px(size * size * 4);
    for (int i = 0; i < size * size; ++i) {
        px[i*4+0] = color.r;
        px[i*4+1] = color.g;
        px[i*4+2] = color.b;
        px[i*4+3] = 255;
    }
    Texture t; t.upload(px.data(), size); return t;
}

// ---------------------------------------------------------------- checker

Texture Texture::checker(int size, int cells, glm::u8vec3 a, glm::u8vec3 b)
{
    std::vector<unsigned char> px(size * size * 4);
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            int cx = (x * cells) / size;
            int cy = (y * cells) / size;
            glm::u8vec3 c = ((cx + cy) & 1) ? a : b;
            int i = (y * size + x) * 4;
            px[i+0] = c.r; px[i+1] = c.g; px[i+2] = c.b; px[i+3] = 255;
        }
    }
    Texture t; t.upload(px.data(), size); return t;
}

// ------------------------------------------------------------------ noise

Texture Texture::noise(int size, unsigned seed, glm::u8vec3 base, int variance)
{
    std::vector<unsigned char> px(size * size * 4);
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            float n = hashF(x, y, seed);
            int v = static_cast<int>(n * variance * 2.0f) - variance;
            int i = (y * size + x) * 4;
            px[i+0] = clamp255(base.r + v);
            px[i+1] = clamp255(base.g + v);
            px[i+2] = clamp255(base.b + v);
            px[i+3] = 255;
        }
    }
    Texture t; t.upload(px.data(), size); return t;
}

// ------------------------------------------------------------------ grass

Texture Texture::grass(int size, unsigned seed)
{
    std::vector<unsigned char> px(size * size * 4);
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            float fx = static_cast<float>(x) / size * 8.0f;
            float fy = static_cast<float>(y) / size * 8.0f;
            float n  = fbm(fx, fy, seed);
            float n2 = fbm(fx * 6.0f, fy * 6.0f, seed ^ 0x5555u);

            int i = (y * size + x) * 4;
            px[i+0] = clamp255(60 + static_cast<int>(n * 35 + n2 * 15));
            px[i+1] = clamp255(95 + static_cast<int>(n * 55 + n2 * 25));
            px[i+2] = clamp255(40 + static_cast<int>(n * 25));
            px[i+3] = 255;
        }
    }
    Texture t; t.upload(px.data(), size); return t;
}

// ------------------------------------------------------------------ brick

Texture Texture::brick(int size, glm::u8vec3 mortar, glm::u8vec3 brick)
{
    std::vector<unsigned char> px(size * size * 4);
    const int rows = 8;
    const int cols = 4;
    const int brickH = size / rows;
    const int brickW = size / cols;
    const int mortarTh = std::max(2, size / 96);

    for (int y = 0; y < size; ++y) {
        int row = y / brickH;
        int offsetX = ((row & 1) != 0) ? (brickW / 2) : 0;

        for (int x = 0; x < size; ++x) {
            int xx = (x + offsetX) % size;
            int bx = xx % brickW;
            int by = y % brickH;
            bool mortarPx = (bx < mortarTh) || (by < mortarTh);

            glm::u8vec3 c = mortarPx ? mortar : brick;

            if (!mortarPx) {
                int delta = static_cast<int>(hashF(x, y, 0xABCDu) * 22.0f) - 11;
                c.r = clamp255(c.r + delta);
                c.g = clamp255(c.g + delta);
                c.b = clamp255(c.b + delta);
            }

            int i = (y * size + x) * 4;
            px[i+0] = c.r; px[i+1] = c.g; px[i+2] = c.b; px[i+3] = 255;
        }
    }
    Texture t; t.upload(px.data(), size); return t;
}

// ------------------------------------------------------------------- wood

Texture Texture::wood(int size, glm::u8vec3 base)
{
    std::vector<unsigned char> px(size * size * 4);
    const int planks = 6;
    const int plankH = size / planks;

    for (int y = 0; y < size; ++y) {
        int plankIdx = y / plankH;
        int py = y % plankH;

        for (int x = 0; x < size; ++x) {
            float fx = static_cast<float>(x) / size * 4.0f;
            float fy = static_cast<float>(y) / size * 28.0f;
            float n  = fbm(fx, fy, 0x1234u * (plankIdx + 1));

            int v = static_cast<int>(n * 40.0f) - 20;
            int i = (y * size + x) * 4;

            glm::u8vec3 c(
                clamp255(base.r + v),
                clamp255(base.g + v),
                clamp255(base.b + v)
            );

            if (py < 2 || py > plankH - 3) {
                c.r = static_cast<unsigned char>(c.r * 0.45f);
                c.g = static_cast<unsigned char>(c.g * 0.45f);
                c.b = static_cast<unsigned char>(c.b * 0.45f);
            }

            px[i+0] = c.r; px[i+1] = c.g; px[i+2] = c.b; px[i+3] = 255;
        }
    }
    Texture t; t.upload(px.data(), size); return t;
}
