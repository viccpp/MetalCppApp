#include "application.h"

#include "renderer.h"
#include "os/bridge.h"

namespace space307 {

constexpr char window_title[] = "Chart application simulator";
constexpr int chart_window_width = 1280;
constexpr int chart_window_height = 720;

Application::Application()
{
}

Application::~Application()
{
}

int Application::run()
{
    // Create the window and CAMetalLayer (the only ObjC call we make)
    create_chart_window(chart_window_width, chart_window_height, window_title);

    // Construct the renderer with the Metal device from the layer
    Renderer renderer(get_rendering_device());
    renderer.mainLoop();

    return 0;
}

} // namespace
