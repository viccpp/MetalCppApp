#include "application.h"
#include <cstdio>
#include <exception>

int main()
{
    try
    {
        space307::Application app;
        return app.run();
    }
    catch(const std::exception& ex)
    {
        std::fprintf(stderr, "%s\n", ex.what());
    }
    return 1;
}
