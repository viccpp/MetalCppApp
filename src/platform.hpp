#pragma once

// Pure C interface — callable from any .cpp file.
// Implemented in platform.mm (the only Objective-C file in the project).

#ifdef __cplusplus
extern "C" {
#endif

void* platform_create_window(int w, int h, const char* title);
void* platform_next_drawable();
void* platform_get_device();
void  platform_poll_events();
bool  platform_should_quit();

#ifdef __cplusplus
}
#endif
