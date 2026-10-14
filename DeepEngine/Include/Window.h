#pragma once
#include <SDL2/SDL.h>
#include <string>

class Window {
public:
    Window(const std::string& title, int width, int height);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool poll();                       // returns false when we should quit
    void swap();
    void setTitle(const std::string& t);
    void grabMouse(bool grab);

    int  width()  const { return m_width; }
    int  height() const { return m_height; }
    bool keyDown(SDL_Scancode sc) const;
    void mouseDelta(float& dx, float& dy);

private:
    SDL_Window*   m_window = nullptr;
    SDL_GLContext m_ctx    = nullptr;
    int m_width  = 0;
    int m_height = 0;

    const Uint8* m_keys = nullptr;
    float m_dx = 0.0f;
    float m_dy = 0.0f;
    bool  m_quit = false;
};
