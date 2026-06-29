#include "platform.hpp"
#include "renderer.hpp"

int main() {
    // Create the window and CAMetalLayer (the only ObjC call we make)
    platform_create_window(1280, 720, "Metal-cpp App");

    // Construct the renderer with the Metal device from the layer
    Renderer renderer(platform_get_device());

    // Run loop
    while (!platform_should_quit()) {
        platform_poll_events();

        void* drawable = platform_next_drawable();
        renderer.draw(drawable);
    }

    return 0;
}
