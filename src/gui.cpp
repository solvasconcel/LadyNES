#include <csignal>
#include <cstring>
#include <dirent.h>
#include <unistd.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "Sound_Queue.h"
#include "apu.hpp"
#include "cartridge.hpp"
#include "cpu.hpp"
#include "menu.hpp"
#include "gui.hpp"
#include "config.hpp"
#include "savestate.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

namespace GUI {

/* Setup ImGui style - DuckStation inspired */
void setup_imgui_style()
{
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Dark theme with anthracite colors
    ImVec4* colors = style.Colors;
    
    colors[ImGuiCol_Text]                   = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_Border]                 = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgActive]           = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_TitleBgActive]           = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.30f, 0.50f, 0.80f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.30f, 0.50f, 0.80f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.35f, 0.55f, 0.85f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_ButtonActive]            = ImVec4(0.25f, 0.45f, 0.75f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.20f, 0.40f, 0.70f, 0.55f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.25f, 0.45f, 0.75f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.30f, 0.50f, 0.80f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.25f, 0.45f, 0.75f, 1.00f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.30f, 0.50f, 0.80f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.30f, 0.50f, 0.80f, 0.50f);
    colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.35f, 0.55f, 0.85f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.60f, 0.90f, 0.95f);
    
    // Padding and spacing - DuckStation style
    style.WindowPadding       = ImVec2(8, 8);
    style.FramePadding        = ImVec2(8, 4);
    style.CellPadding         = ImVec2(4, 2);
    style.ItemSpacing         = ImVec2(8, 4);
    style.ItemInnerSpacing   = ImVec2(4, 4);
    style.TouchExtraPadding  = ImVec2(0, 0);
    style.IndentSpacing      = 20.0f;
    style.ScrollbarSize      = 12.0f;
    style.GrabMinSize        = 8.0f;
    
    // Borders
    style.WindowBorderSize   = 0.0f;
    style.ChildBorderSize    = 0.0f;
    style.PopupBorderSize    = 1.0f;
    style.FrameBorderSize    = 0.0f;
    
    // Rounding
    style.WindowRounding     = 4.0f;
    style.ChildRounding      = 4.0f;
    style.FrameRounding      = 4.0f;
    style.PopupRounding      = 4.0f;
    style.ScrollbarRounding  = 4.0f;
    style.GrabRounding       = 4.0f;
}

// SDL structures:
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* gameTexture;
SDL_Texture* background;
TTF_Font* font;
u8 const* keys;
Sound_Queue* soundQueue;
SDL_Joystick* joystick[] = { nullptr, nullptr };

// Menus:
Menu* menu;
Menu* mainMenu;
Menu* settingsMenu;
Menu* videoMenu;
Menu* keyboardMenu[2];
Menu* joystickMenu[2];
FileMenu* fileMenu;

bool pause = true;
bool fast_forward = false;
bool is_ingame = false;
bool show_rom_browser = false;
const int FAST_FORWARD_MULTIPLIER = 8;

/* Set the window size multiplier */
void set_size(int mul)
{
    last_window_size = mul;
    SDL_SetWindowSize(window, WIDTH * mul, HEIGHT * mul);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

/* Toggle fullscreen mode */
void set_fullscreen(bool enabled)
{
    fullscreen_mode = enabled;
    SDL_SetWindowFullscreen(window, enabled ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

/* Set scaling/filtering mode */
void set_scaling_mode(bool smooth)
{
    scaling_mode = smooth;
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, smooth ? "1" : "0");

    // Recreate game texture for the hint to take effect
    SDL_DestroyTexture(gameTexture);
    gameTexture = SDL_CreateTexture(renderer,
                                    SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                    WIDTH, HEIGHT);

    // Recreate background texture
    SDL_DestroyTexture(background);
    SDL_Surface* backSurface = IMG_Load("res/init.png");
    background = SDL_CreateTextureFromSurface(renderer, backSurface);
    SDL_SetTextureColorMod(background, 60, 60, 60);
    SDL_FreeSurface(backSurface);

    // Recreate all menu entry textures
    auto recreate_menu = [](Menu* m) {
        if (m) {
            for (auto entry : m->entries) {
                entry->set_label(entry->get_label());
            }
        }
    };

    recreate_menu(mainMenu);
    recreate_menu(settingsMenu);
    recreate_menu(videoMenu);
    recreate_menu(keyboardMenu[0]);
    recreate_menu(keyboardMenu[1]);
    if (joystick[0]) recreate_menu(joystickMenu[0]);
    if (joystick[1]) recreate_menu(joystickMenu[1]);
    recreate_menu(fileMenu);
}

/* Toggle aspect ratio stretching */
void set_aspect_stretch(bool enabled)
{
    stretch_aspect = enabled;
    // Only applies in fullscreen mode
    if (fullscreen_mode)
    {
        if (enabled)
        {
            // Disable logical size to stretch to fill screen
            SDL_RenderSetLogicalSize(renderer, 0, 0);
        }
        else
        {
            // Enable logical size for letterboxing
            SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);
        }
    }
}

/* Initialize GUI */
void init()
{
    // Initialize graphics system:
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, scaling_mode ? "1" : "0");

    // Set window class for Wayland/X11 to match .desktop file
    SDL_SetHint("SDL_VIDEO_WAYLAND_WMCLASS", "laines");
    SDL_SetHint("SDL_VIDEO_X11_WMCLASS", "laines");

    TTF_Init();

    for (int i = 0; i < SDL_NumJoysticks(); i++)
        joystick[i] = SDL_JoystickOpen(i);

    APU::init();
    soundQueue = new Sound_Queue;
    soundQueue->init(96000);

    // Initialize graphics structures:
    window      = SDL_CreateWindow  ("LaiNES",
                                     SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                     LAUNCHER_WIDTH, LAUNCHER_HEIGHT, 0);

    // Set window icon:
    SDL_Surface* icon = IMG_Load("res/laines_icon.png");
    if (icon) {
        SDL_SetWindowIcon(window, icon);
        SDL_FreeSurface(icon);
    }

    renderer    = SDL_CreateRenderer(window, -1,
                                     SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    // Don't set logical size for launcher, set later for game

        // Inicialização do ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    
    // Apply DuckStation-like style
    setup_imgui_style();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);


    gameTexture = SDL_CreateTexture (renderer,
                                     SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                     WIDTH, HEIGHT);

    // Apply saved video settings
    if (fullscreen_mode)
    {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        // Apply stretch if enabled in fullscreen
        if (stretch_aspect)
            SDL_RenderSetLogicalSize(renderer, 0, 0);
    }

    font = TTF_OpenFont("res/font.ttf", FONT_SZ);
    keys = SDL_GetKeyboardState(0);

    // Initial background:
    SDL_Surface* backSurface  = IMG_Load("res/init.png");
    background = SDL_CreateTextureFromSurface(renderer, backSurface);
    SDL_SetTextureColorMod(background, 60, 60, 60);
    SDL_FreeSurface(backSurface);

    // Menus:
    mainMenu = new Menu;
    mainMenu->add(new Entry("Load ROM", []{ menu = fileMenu; }));
    mainMenu->add(new Entry("Reset",    []{ Cartridge::reset(); toggle_pause(); }, []{ return Cartridge::loaded(); }));
    mainMenu->add(new Entry("Save State", []{
        std::string filename = SaveState::get_default_filename();
        if (!filename.empty()) {
            SaveState::save(filename.c_str());
        }
        toggle_pause();
    }, []{ return Cartridge::loaded(); }));
    mainMenu->add(new Entry("Load State", []{
        std::string filename = SaveState::get_default_filename();
        if (!filename.empty()) {
            SaveState::load(filename.c_str());
        }
        toggle_pause();
    }, []{ return Cartridge::loaded(); }));
    mainMenu->add(new Entry("Settings", []{ menu = settingsMenu; }));
    mainMenu->add(new Entry("Exit",     []{ exit(0); }));

    settingsMenu = new Menu;
    settingsMenu->add(new Entry("<",            []{ menu = mainMenu; }));
    settingsMenu->add(new Entry("Video",        []{ menu = videoMenu; }));
    settingsMenu->add(new Entry("Controller 1", []{ menu = useJoystick[0] ? joystickMenu[0] : keyboardMenu[0]; }));
    settingsMenu->add(new Entry("Controller 2", []{ menu = useJoystick[1] ? joystickMenu[1] : keyboardMenu[1]; }));
    settingsMenu->add(new Entry("Save Settings", []{ save_settings(); menu = mainMenu; }));

    videoMenu = new Menu;
    videoMenu->add(new Entry("<",       []{ menu = settingsMenu; }));
    videoMenu->add(new CycleEntry("Size: ", &last_window_size, 1, 6, [](int v){ set_size(v); }, "x", []{ return !fullscreen_mode; }));
    videoMenu->add(new ToggleEntry("Fullscreen: ", &fullscreen_mode, [](bool v){ set_fullscreen(v); }));
    videoMenu->add(new ToggleEntry("Smooth Scaling: ", &scaling_mode, [](bool v){ set_scaling_mode(v); }));
    videoMenu->add(new ToggleEntry("Stretch Aspect: ", &stretch_aspect, [](bool v){ set_aspect_stretch(v); }, []{ return fullscreen_mode; }));

    for (int i = 0; i < 2; i++)
    {
        keyboardMenu[i] = new Menu;
        keyboardMenu[i]->add(new Entry("<", []{ menu = settingsMenu; }));
        if (joystick[i] != nullptr)
            keyboardMenu[i]->add(new Entry("Joystick >", [=]{ menu = joystickMenu[i]; useJoystick[i] = true; }));
        keyboardMenu[i]->add(new ControlEntry("Up",     &KEY_UP[i]));
        keyboardMenu[i]->add(new ControlEntry("Down",   &KEY_DOWN[i]));
        keyboardMenu[i]->add(new ControlEntry("Left",   &KEY_LEFT[i]));
        keyboardMenu[i]->add(new ControlEntry("Right",  &KEY_RIGHT[i]));
        keyboardMenu[i]->add(new ControlEntry("A",      &KEY_A[i]));
        keyboardMenu[i]->add(new ControlEntry("B",      &KEY_B[i]));
        keyboardMenu[i]->add(new ControlEntry("Start",  &KEY_START[i]));
        keyboardMenu[i]->add(new ControlEntry("Select", &KEY_SELECT[i]));

        if (joystick[i] != nullptr)
        {
            joystickMenu[i] = new Menu;
            joystickMenu[i]->add(new Entry("<", []{ menu = settingsMenu; }));
            joystickMenu[i]->add(new Entry("< Keyboard", [=]{ menu = keyboardMenu[i]; useJoystick[i] = false; }));
            joystickMenu[i]->add(new ControlEntry("Up",     &BTN_UP[i]));
            joystickMenu[i]->add(new ControlEntry("Down",   &BTN_DOWN[i]));
            joystickMenu[i]->add(new ControlEntry("Left",   &BTN_LEFT[i]));
            joystickMenu[i]->add(new ControlEntry("Right",  &BTN_RIGHT[i]));
            joystickMenu[i]->add(new ControlEntry("A",      &BTN_A[i]));
            joystickMenu[i]->add(new ControlEntry("B",      &BTN_B[i]));
            joystickMenu[i]->add(new ControlEntry("Start",  &BTN_START[i]));
            joystickMenu[i]->add(new ControlEntry("Select", &BTN_SELECT[i]));
        }
    }

    fileMenu = new FileMenu;

    menu = mainMenu;
}

/* Render a texture on screen */
void render_texture(SDL_Texture* texture, int x, int y)
{
    int w, h;
    SDL_Rect dest;

    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
    if (x == TEXT_CENTER)
        dest.x = WIDTH/2 - dest.w/2;
    else if (x == TEXT_RIGHT)
        dest.x = WIDTH - dest.w - 10;
    else
        dest.x = x + 10;
    dest.y = y + 5;

    SDL_RenderCopy(renderer, texture, NULL, &dest);
}

/* Generate a texture from text */
SDL_Texture* gen_text(std::string text, SDL_Color color)
{
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);
    return texture;
}

/* Get the joypad state from SDL */
u8 get_joypad_state(int n)
{
    const int DEAD_ZONE = 8000;

    u8 j = 0;
    
    // Get normal controller input first
    if (useJoystick[n])
    {
        j |= (SDL_JoystickGetButton(joystick[n], BTN_A[n]))      << 0;  // A.
        j |= (SDL_JoystickGetButton(joystick[n], BTN_B[n]))      << 1;  // B.
        j |= (SDL_JoystickGetButton(joystick[n], BTN_SELECT[n])) << 2;  // Select.
        j |= (SDL_JoystickGetButton(joystick[n], BTN_START[n]))  << 3;  // Start.

        j |= (SDL_JoystickGetButton(joystick[n], BTN_UP[n]))     << 4;  // Up.
        j |= (SDL_JoystickGetAxis(joystick[n], 1) < -DEAD_ZONE)  << 4;
        j |= (SDL_JoystickGetButton(joystick[n], BTN_DOWN[n]))   << 5;  // Down.
        j |= (SDL_JoystickGetAxis(joystick[n], 1) >  DEAD_ZONE)  << 5;
        j |= (SDL_JoystickGetButton(joystick[n], BTN_LEFT[n]))   << 6;  // Left.
        j |= (SDL_JoystickGetAxis(joystick[n], 0) < -DEAD_ZONE)  << 6;
        j |= (SDL_JoystickGetButton(joystick[n], BTN_RIGHT[n]))  << 7;  // Right.
        j |= (SDL_JoystickGetAxis(joystick[n], 0) >  DEAD_ZONE)  << 7;
    }
    else
    {
        j |= (keys[KEY_A[n]])      << 0;
        j |= (keys[KEY_B[n]])      << 1;
        j |= (keys[KEY_SELECT[n]]) << 2;
        j |= (keys[KEY_START[n]])  << 3;
        j |= (keys[KEY_UP[n]])     << 4;
        j |= (keys[KEY_DOWN[n]])   << 5;
        j |= (keys[KEY_LEFT[n]])   << 6;
        j |= (keys[KEY_RIGHT[n]])  << 7;
    }
    
    return j;
}

/* Send the rendered frame to the GUI */
void new_frame(u32* pixels)
{
    SDL_UpdateTexture(gameTexture, NULL, pixels, WIDTH * sizeof(u32));
}

void new_samples(const blip_sample_t* samples, size_t count)
{
    // Skip audio during fast forward to prevent buffer blocking
    if (!fast_forward) {
        soundQueue->write(samples, count);
    }
}

bool is_fast_forward()
{
    return fast_forward;
}

/* Render the screen */
void render()
{
    SDL_RenderClear(renderer);

    // Draw the NES screen or background:
    if (Cartridge::loaded()) {
        SDL_RenderCopy(renderer, gameTexture, NULL, NULL);
    } else {
        // Launcher mode - clear with neutral color
        SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
        SDL_RenderClear(renderer);
    }

    // Draw fast forward indicator (only when in-game and not paused):
    if (fast_forward && Cartridge::loaded() && !pause) {
        SDL_Texture* ff_indicator = gen_text(">>", { 255, 255, 0 });
        render_texture(ff_indicator, TEXT_RIGHT, 0);
        SDL_DestroyTexture(ff_indicator);
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

    SDL_RenderPresent(renderer);
}

/* Play/stop the game */
void toggle_pause()
{
    pause = not pause;

    // Disable fast forward when pausing
    if (pause)
        fast_forward = false;

    if (pause)
        SDL_SetTextureColorMod(gameTexture,  60,  60,  60);
    else
        SDL_SetTextureColorMod(gameTexture, 255, 255, 255);
}

/* Set pause state directly */
void set_paused(bool paused)
{
    pause = paused;

    // Disable fast forward when pausing
    if (pause)
        fast_forward = false;

    if (pause)
        SDL_SetTextureColorMod(gameTexture,  60,  60,  60);
    else
        SDL_SetTextureColorMod(gameTexture, 255, 255, 255);
}

/* Close the current game */
void close_game()
{
    Cartridge::unload();
    pause = true;
    fast_forward = false;
}

/* Prompt for a key, return the scancode */
SDL_Scancode query_key()
{
    SDL_Texture* prompt = gen_text("Press a key...", { 255, 255, 255 });
    render_texture(prompt, TEXT_CENTER, HEIGHT - FONT_SZ*4);
    SDL_RenderPresent(renderer);

    SDL_Event e;
    while (true)
    {
        SDL_PollEvent(&e);
        if (e.type == SDL_KEYDOWN)
            return e.key.keysym.scancode;
    }
}

int query_button()
{
    SDL_Texture* prompt = gen_text("Press a button...", { 255, 255, 255 });
    render_texture(prompt, TEXT_CENTER, HEIGHT - FONT_SZ*4);
    SDL_RenderPresent(renderer);

    SDL_Event e;
    while (true)
    {
        SDL_PollEvent(&e);
        if (e.type == SDL_JOYBUTTONDOWN)
            return e.jbutton.button;
    }
}

/* Run the emulator */
/* Render the launcher interface */
void render_launcher()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Launcher", nullptr, 
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    // Title
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.1f);
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("SosiNES").x * 2) * 0.5f);
    ImGui::TextColored(ImVec4(0.30f, 0.50f, 0.80f, 1.0f), "SosiNES");
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Emulador NES Moderno").x) * 0.5f);
    ImGui::Text("Emulador NES Moderno");

    // Columns for Actions and Settings
    ImGui::Columns(2, "LauncherColumns", false);
    ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.5f);

    // Left Panel: Actions
    ImGui::Text("Ações");
    ImGui::Separator();
    if (ImGui::Button("Load ROM", ImVec2(-1, 40))) {
        show_rom_browser = true;
    }

    ImGui::NextColumn();

    // Right Panel: Settings
    ImGui::Text("Configurações");
    ImGui::Separator();
    if (ImGui::Button("Engrenagem", ImVec2(-1, 40))) {
        ImGui::OpenPopup("Settings");
    }
    if (ImGui::BeginPopup("Settings")) {
        ImGui::Checkbox("Fullscreen", &fullscreen_mode);
        if (ImGui::IsItemEdited()) set_fullscreen(fullscreen_mode);
        ImGui::Checkbox("Aspect Ratio Stretch", &stretch_aspect);
        if (ImGui::IsItemEdited()) set_aspect_stretch(stretch_aspect);
        ImGui::Checkbox("Smooth Scaling", &scaling_mode);
        if (ImGui::IsItemEdited()) set_scaling_mode(scaling_mode);
        ImGui::EndPopup();
    }

    ImGui::Columns(1);

    ImGui::End();

    // ROM Browser Modal
    if (show_rom_browser) {
        ImGui::OpenPopup("Selecionar ROM");
        show_rom_browser = false;
    }
    if (ImGui::BeginPopupModal("Selecionar ROM", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char current_dir[512];
        static bool dir_initialized = false;
        
        if (!dir_initialized) {
            getcwd(current_dir, 512);
            dir_initialized = true;
        }
        
        ImGui::Text("Diretório: %s", current_dir);
        ImGui::Separator();
        
        // List directories and .nes files
        DIR* dp = opendir(current_dir);
        if (dp) {
            struct dirent* dirp;
            while ((dirp = readdir(dp)) != nullptr) {
                std::string name = dirp->d_name;
                std::string full_path = std::string(current_dir) + "/" + name;
                
                if (name[0] == '.' && name != "..") continue;
                
                if (dirp->d_type == DT_DIR) {
                    if (ImGui::Selectable((name + "/").c_str())) {
                        strcpy(current_dir, full_path.c_str());
                    }
                } else if (name.size() > 4 && name.substr(name.size() - 4) == ".nes") {
                    if (ImGui::Selectable(name.c_str())) {
                        // Load the ROM
                        Cartridge::load(full_path.c_str());
                        if (Cartridge::loaded()) {
                            pause = false;
                            set_size(last_window_size); // Resize to game size
                            ImGui::CloseCurrentPopup();
                        }
                    }
                }
            }
            closedir(dp);
        }
        
        ImGui::Separator();
        if (ImGui::Button("Fechar")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

/* Render the pause menu overlay */
void render_pause_menu()
{
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f - 150, ImGui::GetIO().DisplaySize.y * 0.5f - 150));
    ImGui::SetNextWindowSize(ImVec2(300, 250));
    ImGui::Begin("Pause Menu", nullptr, 
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    if (ImGui::Button("Resume", ImVec2(-1, 40))) {
        pause = false;
    }
    if (ImGui::Button("Save State", ImVec2(-1, 40))) {
        std::string filename = SaveState::get_default_filename();
        if (!filename.empty()) {
            SaveState::save(filename.c_str());
        }
    }
    if (ImGui::Button("Load State", ImVec2(-1, 40))) {
        std::string filename = SaveState::get_default_filename();
        if (!filename.empty()) {
            SaveState::load(filename.c_str());
        }
    }
    ImGui::Separator();
    ImGui::Text("Quick Settings");
    ImGui::Checkbox("Fullscreen", &fullscreen_mode);
    if (ImGui::IsItemEdited()) set_fullscreen(fullscreen_mode);
    ImGui::Checkbox("Aspect Ratio Stretch", &stretch_aspect);
    if (ImGui::IsItemEdited()) set_aspect_stretch(stretch_aspect);
    ImGui::Checkbox("Smooth Scaling", &scaling_mode);
    if (ImGui::IsItemEdited()) set_scaling_mode(scaling_mode);

    ImGui::Separator();
    if (ImGui::Button("Close ROM", ImVec2(-1, 40))) {
        close_game();
        // Resize back to launcher
        SDL_SetWindowSize(window, LAUNCHER_WIDTH, LAUNCHER_HEIGHT);
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }

    ImGui::End();
}

void run()
{
    SDL_Event e;
    u32 frameStart, frameTime;
    const int FPS   = 60;
    const int DELAY = 1000.0f / FPS;

    while (true)
    {
        frameStart = SDL_GetTicks();

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Always process ImGui events
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT) return;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE && Cartridge::loaded() && !pause) {
                    pause = true;
                }
            }
        }

        // State machine
        if (!Cartridge::loaded()) {
            // LAUNCHER STATE
            render_launcher();
        } else if (Cartridge::loaded() && !pause) {
            // GAMEPLAY STATE
            fast_forward = keys[SDL_SCANCODE_TAB];
            int frames_to_run = fast_forward ? FAST_FORWARD_MULTIPLIER : 1;
            for (int i = 0; i < frames_to_run; i++) {
                CPU::run_frame();
            }
        } else if (Cartridge::loaded() && pause) {
            // PAUSE STATE
            render_pause_menu();
        }

        render();

        if (!fast_forward) {
            frameTime = SDL_GetTicks() - frameStart;
            if (frameTime < DELAY)
                SDL_Delay((int)(DELAY - frameTime));
        }
    }
}

}
