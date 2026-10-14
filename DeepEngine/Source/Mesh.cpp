#include <Mesh.h>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    m_count = static_cast<GLsizei>(indices.size());

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
                 indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, uv)));

    glBindVertexArray(0);
}

Mesh::~Mesh() { release(); }

Mesh::Mesh(Mesh&& o) noexcept
    : m_vao(o.m_vao), m_vbo(o.m_vbo), m_ebo(o.m_ebo), m_count(o.m_count)
{
    o.m_vao = o.m_vbo = o.m_ebo = 0;
    o.m_count = 0;
}

Mesh& Mesh::operator=(Mesh&& o) noexcept
{
    if (this != &o) {
        release();
        m_vao = o.m_vao; m_vbo = o.m_vbo; m_ebo = o.m_ebo; m_count = o.m_count;
        o.m_vao = o.m_vbo = o.m_ebo = 0; o.m_count = 0;
    }
    return *this;
}

void Mesh::release()
{
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    m_ebo = m_vbo = m_vao = 0;
    m_count = 0;
}

void Mesh::draw() const
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_count, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

Mesh Mesh::createCube()
{
    // Unit cube centered at origin, edge length 1 (-0.5 .. +0.5)
    const std::vector<Vertex> v = {
        // +X
        {{ 0.5f,-0.5f,-0.5f}, { 1, 0, 0}, {0,0}},
        {{ 0.5f, 0.5f,-0.5f}, { 1, 0, 0}, {0,1}},
        {{ 0.5f, 0.5f, 0.5f}, { 1, 0, 0}, {1,1}},
        {{ 0.5f,-0.5f, 0.5f}, { 1, 0, 0}, {1,0}},
        // -X
        {{-0.5f,-0.5f, 0.5f}, {-1, 0, 0}, {0,0}},
        {{-0.5f, 0.5f, 0.5f}, {-1, 0, 0}, {0,1}},
        {{-0.5f, 0.5f,-0.5f}, {-1, 0, 0}, {1,1}},
        {{-0.5f,-0.5f,-0.5f}, {-1, 0, 0}, {1,0}},
        // +Y
        {{-0.5f, 0.5f,-0.5f}, { 0, 1, 0}, {0,0}},
        {{-0.5f, 0.5f, 0.5f}, { 0, 1, 0}, {0,1}},
        {{ 0.5f, 0.5f, 0.5f}, { 0, 1, 0}, {1,1}},
        {{ 0.5f, 0.5f,-0.5f}, { 0, 1, 0}, {1,0}},
        // -Y
        {{-0.5f,-0.5f, 0.5f}, { 0,-1, 0}, {0,0}},
        {{-0.5f,-0.5f,-0.5f}, { 0,-1, 0}, {0,1}},
        {{ 0.5f,-0.5f,-0.5f}, { 0,-1, 0}, {1,1}},
        {{ 0.5f,-0.5f, 0.5f}, { 0,-1, 0}, {1,0}},
        // +Z
        {{-0.5f,-0.5f, 0.5f}, { 0, 0, 1}, {0,0}},
        {{ 0.5f,-0.5f, 0.5f}, { 0, 0, 1}, {1,0}},
        {{ 0.5f, 0.5f, 0.5f}, { 0, 0, 1}, {1,1}},
        {{-0.5f, 0.5f, 0.5f}, { 0, 0, 1}, {0,1}},
        // -Z
        {{ 0.5f,-0.5f,-0.5f}, { 0, 0,-1}, {0,0}},
        {{-0.5f,-0.5f,-0.5f}, { 0, 0,-1}, {1,0}},
        {{-0.5f, 0.5f,-0.5f}, { 0, 0,-1}, {1,1}},
        {{ 0.5f, 0.5f,-0.5f}, { 0, 0,-1}, {0,1}},
    };

    std::vector<unsigned int> idx;
    idx.reserve(36);
    for (unsigned int f = 0; f < 6; ++f) {
        unsigned int b = f * 4;
        idx.push_back(b + 0); idx.push_back(b + 1); idx.push_back(b + 2);
        idx.push_back(b + 0); idx.push_back(b + 2); idx.push_back(b + 3);
    }

    return Mesh(v, idx);
}

Mesh Mesh::createPlane(float size)
{
    float h = size * 0.5f;
    const std::vector<Vertex> v = {
        {{-h, 0.0f, -h}, {0, 1, 0}, {0, 0}},
        {{ h, 0.0f, -h}, {0, 1, 0}, {1, 0}},
        {{ h, 0.0f,  h}, {0, 1, 0}, {1, 1}},
        {{-h, 0.0f,  h}, {0, 1, 0}, {0, 1}},
    };
    const std::vector<unsigned int> idx = { 0, 2, 1, 0, 3, 2 };
    return Mesh(v, idx);
}
