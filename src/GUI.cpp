#include <tuple>

#include ".\include\GUI.h"

#include ".\imgui.h"
#include ".\imgui_impl_sdl.h"
#include ".\imgui_impl_opengl3.h"

#include ".\imgui_memory_editor\imgui_memory_editor.h"
#include ".\imfilebrowser.h"

#include ".\SDL.h"
#include ".\SDL_syswm.h"
#include ".\GL\gl3w.h"

#include ".\include\Cartridge.h"
#include ".\include\Mapper_Collection.h"

GUIClass::GUIClass(std::string progName) {

    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {

        std::cerr << "Error: " << SDL_GetError() << '\n' << std::flush;
        exit(-1);

    }

    // Decide GL+GLSL versions
#ifdef __APPLE__
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    this->window = SDL_CreateWindow(progName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    renderer = SDL_CreateRenderer(window, -1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(renderer, 1280, 720);
    this->gl_context = SDL_GL_CreateContext(this->window);
    SDL_GL_MakeCurrent(this->window, this->gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    if (gl3wInit() != 0)
        std::cerr << "Failed to initialize GL3W!\n" << std::flush;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(this->window, this->gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    //  initialize filebrowser for title and file type lookup
    this->filebrowser.SetTitle("Choose File");
    this->filebrowser.SetTypeFilters({ ".nes", ".*" });

}

GUIClass::~GUIClass() {

	//	we need to deallocate the memory used for the memory editor windows
	for (uint32_t i = 0; i < this->cart_hex_windows.size(); i++)
		delete this->cart_hex_windows.at(i).hex_view;
    for (uint32_t i = 0; i < this->cpu_hex_windows.size(); i++)
        delete this->cpu_hex_windows.at(i).hex_view;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(this->gl_context);
	SDL_DestroyWindow(this->window);
	SDL_Quit();

}

void GUIClass::addCartViewer(std::string windowName, CartridgeClass::GetCartData getData_ptr, CartridgeClass::getCartDataSize getSize_ptr) {

    this->cart_hex_windows.push_back(CartridgeMemoryEditorContainer());
    this->cart_hex_windows.at(this->cart_hex_windows.size() - 1).hex_view = new MemoryEditor;
    this->cart_hex_windows.at(this->cart_hex_windows.size() - 1).hex_name = windowName;
    this->cart_hex_windows.at(this->cart_hex_windows.size() - 1).getDataFunc = getData_ptr;
    this->cart_hex_windows.at(this->cart_hex_windows.size() - 1).getSizeFunc = getSize_ptr;

}

void GUIClass::addCPUViewer(std::string windowName, CPUClass::getCPUData getData_ptr, CPUClass::getCPUDataSize getSize_ptr) {

    this->cpu_hex_windows.push_back(CPUMemoryEditorContainer());
    this->cpu_hex_windows.at(this->cpu_hex_windows.size() - 1).hex_view = new MemoryEditor;
    this->cpu_hex_windows.at(this->cpu_hex_windows.size() - 1).hex_name = windowName;
    this->cpu_hex_windows.at(this->cpu_hex_windows.size() - 1).getDataFunc = getData_ptr;
    this->cpu_hex_windows.at(this->cpu_hex_windows.size() - 1).getSizeFunc = getSize_ptr;

}

void GUIClass::draw() {
    
    ImGuiIO& io = ImGui::GetIO();
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //  check for events, handle ones we want to here, specifically for quitting
    SDL_Event event;
    while (SDL_PollEvent(&event)) {

        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            isRendering = false;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            isRendering = false;

    }

	//	begin new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(this->window);
	ImGui::NewFrame();

    //  draw the menu now before dealing with sub windows
    this->drawMenu();

    if (showSelectDebug)
        this->drawSelectDebug();

    if (this->showDebug)
        this->drawDebug();

    if (showFileDialog)
        this->drawFileDialog();

    // Rendering
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(this->window);
    //SDL_RenderPresent(this->renderer);

}

uint8_t GUIClass::getControllerState() {

    const uint32_t dead_zone = 8000;

    uint8_t temp = 0;

    temp |= SDLK_r;         //  A pressed
    temp |= SDLK_f << 1;    //  B pressed
    temp |= SDLK_t << 2;    //  select pressed
    temp |= SDLK_g << 3;    //  start pressed
    temp |= SDLK_w << 4;    //  up pressed
    temp |= SDLK_s << 5;    //  down pressed
    temp |= SDLK_a << 6;    //  left pressed
    temp |= SDLK_d << 7;    //  right pressed

    return temp;

}

bool GUIClass::isLoaded() {

    if (this->cartridge != nullptr)
        return this->cartridge->loaded();
    return false;

}

void GUIClass::updateFrame(uint32_t* pixels) {

    //  need to adjust last parameter(pitch) based on PPU information(dimensions of the screen to be rendered)
    SDL_UpdateTexture(gameTexture, NULL, pixels, 0);

}

void GUIClass::drawMenu() {

    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("File")) {

        if (ImGui::MenuItem("Load ROM")) {

            //  it might be possible to cause issues by attempting to "load ROM" multiple times without closing the previous ImGuiFileDialog window
                //  checked: because of the first parameter which acts as a key, no more than one window with the same key will open, so this issue will not arise

            //  set to true so the GUI::Draw function knows to begin displaying this dialog
            showFileDialog = true;
            //  as far as I know, there is only the *.nes extension for ROM files, so we can tell it to only look for that file extension
            
            //ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDialog", "Choose File", ".nes,.*", ".");
            this->filebrowser.Open();

        }
        if (this->isLoaded()) {

            if (ImGui::MenuItem("Unload ROM")) {

                this->cartridge->unload();
                this->loadedFile = "";

            }

        }

        ImGui::EndMenu();

    }
    if (ImGui::BeginMenu("System")) {

        //  we can use this to render but not do emulations
        //  effectively, stopping the CPU from doing any more executions
        //  we do two different ones for better UX, allowing the user to know if emulation is paused or not
        if (this->isPaused) {

            if (ImGui::MenuItem("Unpause"))
                this->isPaused = !this->isPaused;

        }
        else {

            if (ImGui::MenuItem("Pause"))
                this->isPaused = !this->isPaused;

        }

        //  moved to sub-menu item so if user drops one menu and drags into previous Quit menu button, it does not auto-quit
        if (ImGui::MenuItem("Quit"))
            this->isRendering = false;

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Debug")) {

        if (ImGui::MenuItem("Select Debug Windows"))
            showSelectDebug = true;

        //  we can toggle if we want to show the debugging windows or not with this
        //  same with the pause menu button, we do two different ones for better UX
        if (this->showDebug) {

            if (ImGui::MenuItem("Hide Debug Windows"))
                this->showDebug = !this->showDebug;

        }
        else {

            if (ImGui::MenuItem("Show Debug Windows"))
                this->showDebug = !this->showDebug;

        }

        ImGui::EndMenu();

    }

    ImGui::EndMainMenuBar();

}

void GUIClass::drawSelectDebug() {

    //  creates the checkbox for each stored MemoryEditor we have currently
    //  this will grow on the fly if we decide to allow the complete removal/addition of sections by the user
    ImGui::Begin("Debug Window Select", &showSelectDebug);
    for (uint32_t i = 0; i < this->cart_hex_windows.size(); i++)
        ImGui::Checkbox(this->cart_hex_windows.at(i).hex_name.c_str(), &this->cart_hex_windows.at(i).isShown);
    for (uint32_t i = 0; i < this->cpu_hex_windows.size(); i++)
        ImGui::Checkbox(this->cpu_hex_windows.at(i).hex_name.c_str(), &this->cpu_hex_windows.at(i).isShown);
    ImGui::End();

}

void GUIClass::drawDebug() {

    ImGui::Begin("Information");
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Loaded File: %s", this->loadedFile.c_str());
    ImGui::End();

    //  here we use the function pointers we stored before to grab the data we wish to view, if it is available
    for (uint32_t i = 0; i < cart_hex_windows.size(); i++)
        if (this->cart_hex_windows.at(i).isShown && (this->cartridge->*this->cart_hex_windows.at(i).getDataFunc)() != nullptr)
            this->cart_hex_windows.at(i).hex_view->DrawWindow(this->cart_hex_windows.at(i).hex_name.c_str(), (this->cartridge->*this->cart_hex_windows.at(i).getDataFunc)(), (this->cartridge->*this->cart_hex_windows.at(i).getSizeFunc)());
    for (uint32_t i = 0; i < cpu_hex_windows.size(); i++)
        //  CPU sections will always be available, therefore we do not need to check if these sections actually exist or not
        if (this->cpu_hex_windows.at(i).isShown)
            this->cpu_hex_windows.at(i).hex_view->DrawWindow(this->cpu_hex_windows.at(i).hex_name.c_str(), (this->CPU->*this->cpu_hex_windows.at(i).getDataFunc)(), (this->CPU->*this->cpu_hex_windows.at(i).getSizeFunc)());

}

void GUIClass::drawFileDialog() {

    //  "Load ROM" buttonw as pressed, therefore we need to display the file directory to choose what rom file to load

    filebrowser.Display();

    if (filebrowser.HasSelected()) {

        this->loadedFile = filebrowser.GetSelected().string();

        if (loadedFile.find(".nes") != std::string::npos && ((loadedFile.find_last_of(".") - loadedFile.find_last_of("\\")) > 1)) {

            //  grab the file path name and load the new ROM in
            this->cartridge->storeMapper(selectMapper(this->cartridge->load(this->loadedFile)));

            //  store just the file name itself
            this->loadedFile = loadedFile.substr(loadedFile.find_last_of("\\") + 1);

            //  reset the CPU to a known state to start the new ROM
            //  emulation requires the cartridge ROM be loaded before the CPU can be started/restarted
            //  because the CPU must read a specific section of the ROM(0xFFFC and 0xFFFD) to find where to begin execution
            this->CPU->reset();

            //  we would need to reset all components here that require it(APU and PPU specifically)

            //  check if a texture is already made, if so, destroy it so we do not leak memory
            if (this->gameTexture)
                SDL_DestroyTexture(this->gameTexture);

            //  check if we have the PPU loaded, if not, we can ignore this
            if (this->PPU) {

                uint32_t x, y;
                std::tie(x, y) = this->PPU->getResolution();
                this->gameTexture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, x, y);

            }

        }
        else
            this->loadedFile = "";

        showFileDialog = false;

    }

}