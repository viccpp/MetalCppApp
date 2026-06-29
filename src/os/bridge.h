#pragma once

// Pure C interface — callable from any .cpp file.
// Implemented in os/bridge.mm (the only Objective-C++ file in the project).

extern "C" {

void* create_chart_window(int w, int h, const char* title);
void* get_next_drawable();
void* get_rendering_device();
void  poll_application_events();
bool  application_should_quit();

} // extern "C"
