#include "gui.hpp"
#include "config.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

int main(int argc, char *argv[])
{
    GUI::load_settings();
    GUI::init();
    GUI::run();

    return 0;
}
