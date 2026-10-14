#include <Window.h>
#include <GL/glew.h>
#include <stdexcept>

Window::Window(const std::string& title, int width, int height)
    : m_width(width), m_height(height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    m_window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL);

    if (!m_window)
        throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());

    m_ctx = SDL_GL_CreateContext(m_window);
    if (!m_ctx)
        throw std::runtime_error(std::string("SDL_GL_CreateContext failed: ") + SDL_GetError());

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
        throw std::runtime_error(std::string("glewInit failed: ") +
                                 reinterpret_cast<const char*>(glewGetErrorString(err)));
    // GLEW sometimes leaves a stray error on core profiles
    while (glGetError() != GL_NO_ERROR) {}

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    SDL_GL_SetSwapInterval(1); // vsync
    SDL_SetRelativeMouseMode(SDL_TRUE);

    m_keys = SDL_GetKeyboardState(nullptr);
}

Window::~Window()
{
    if (m_ctx)    SDL_GL_DeleteContext(m_ctx);
    if (m_window) SDL_DestroyWindow(m_window);
    SDL_Quit();
}

bool Window::poll()
{
    m_dx = m_dy = 0.0f;

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            m_quit = true;
        }
        else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE)
                m_quit = true;
        }
        else if (e.type == SDL_MOUSEMOTION) {
            m_dx += static_cast<float>(e.motion.xrel);
            m_dy += static_cast<float>(e.motion.yrel);
        }
    }

    m_keys = SDL_GetKeyboardState(nullptr);
    return !m_quit;
}

void Window::swap()
{
    SDL_GL_SwapWindow(m_window);
}

void Window::setTitle(const std::string& t)
{
    SDL_SetWindowTitle(m_window, t.c_str());
}

void Window::grabMouse(bool grab)
{
    SDL_SetRelativeMouseMode(grab ? SDL_TRUE : SDL_FALSE);
}

bool Window::keyDown(SDL_Scancode sc) const
{
    return m_keys && m_keys[sc];
}

void Window::mouseDelta(float& dx, float& dy)
{
    dx = m_dx;
    dy = m_dy;
}
