#pragma once
#include <cstdint>

// Pure C++ interface — no Metal-cpp types leak into this header.
// The implementation (renderer.cpp) is the only place that touches Metal.

class Renderer {
public:
    explicit Renderer(void* metalDevice);
    ~Renderer();

    void resize(float width, float height);
    void draw(void* nextDrawable);

private:
    struct Impl;
    Impl* _impl;
};
