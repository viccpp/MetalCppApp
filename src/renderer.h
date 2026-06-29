#pragma once

// Pure C++ interface — no Metal-cpp types leak into this header.
// The implementation (renderer.cpp) is the only place that touches Metal.

#include <memory>

namespace space307 {

class Renderer
{
public:
    explicit Renderer(void* metalDevice);
    ~Renderer();

    void resize(float width, float height);

    void mainLoop();

private:
    void draw(void* nextDrawable);

    struct Impl;
    std::unique_ptr<Impl> _impl;
};

} // namespace
