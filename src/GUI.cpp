#include <tuple>

#include ".\include\GUI.h"

#include ".\imgui.h"
#include ".\imgui-SFML.h"

#include ".\SFML\Audio.hpp"
#include ".\SFML\Graphics.hpp"
#include ".\SFML\System.hpp"
#include ".\SFML\Window.hpp"
#include ".\SFML\Window\Joystick.hpp"

#include ".\SFML\Graphics\Drawable.hpp"

#include ".\imgui_memory_editor\imgui_memory_editor.h"
#include ".\imfilebrowser.h"


#include ".\include\Cartridge.h"
#include ".\include\Mapper_Collection.h"

#include ".\include\Controller.h"

GUIClass::GUIClass(std::string progName) {

    //  initialize SFML window context to a default state
    this->window.create(sf::VideoMode(256, 240), progName);
    this->window.setFramerateLimit(60);

    this->gameTexture.create(1280, 720);

    ImGui::SFML::Init(this->window);

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
    for (uint32_t i = 0; i < this->ppu_hex_windows.size(); i++)
        delete this->ppu_hex_windows.at(i).hex_view;

    ImGui::SFML::Shutdown();

}

void GUIClass::addCartViewer(std::string windowName, CartridgeClass::GetCartData getData_ptr, CartridgeClass::getCartDataSize getSize_ptr) {

    this->cart_hex_windows.push_back(CartridgeMemoryEditorContainer());
    this->cart_hex_windows.back().hex_view = new MemoryEditor;
    this->cart_hex_windows.back().hex_name = windowName;
    this->cart_hex_windows.back().getDataFunc = getData_ptr;
    this->cart_hex_windows.back().getSizeFunc = getSize_ptr;

}

void GUIClass::addCPUViewer(std::string windowName, CPUClass::getCPUData getData_ptr, CPUClass::getCPUDataSize getSize_ptr) {

    this->cpu_hex_windows.push_back(CPUMemoryEditorContainer());
    this->cpu_hex_windows.back().hex_view = new MemoryEditor;
    this->cpu_hex_windows.back().hex_name = windowName;
    this->cpu_hex_windows.back().getDataFunc = getData_ptr;
    this->cpu_hex_windows.back().getSizeFunc = getSize_ptr;

}

void GUIClass::addPPUViewer(std::string windowName, PPUClass::getPPUData getData_ptr, PPUClass::getPPUDataSize getSize_ptr) {

    this->ppu_hex_windows.push_back(PPUMemoryEditorContainer());
    this->ppu_hex_windows.back().hex_view = new MemoryEditor;
    this->ppu_hex_windows.back().hex_name = windowName;
    this->ppu_hex_windows.back().getDataFunc = getData_ptr;
    this->ppu_hex_windows.back().getSizeFunc = getSize_ptr;

}

void GUIClass::draw() {

    sf::Event e;
    while (this->window.pollEvent(e)) {

        ImGui::SFML::ProcessEvent(e);

        if (e.type == sf::Event::Closed)
            this->isRendering = false;

    }

    sf::Sprite sprite(this->gameTexture);

    ImGui::SFML::Update(this->window, clock.restart());

    //sprite.setColor(sf::Color::Blue);
    //ImGui::Image(sprite);

    //  draw the menu now before dealing with sub windows
    this->drawMenu();

    if (showSelectDebug)
        this->drawSelectDebug();

    if (this->showDebug)
        this->drawDebug();

    if (showMapperError)
        this->drawErrorWindow();

    if (showFileDialog)
        this->drawFileDialog();
    if (showControllerDialog)
        this->drawControllerDialog();
    if (showButtonSet[0])
        this->drawSetButtons(0);
    if (showButtonSet[1])
        this->drawSetButtons(1);
    if (showButtonSet[2])
        this->drawSetButtons(2);
    if (showButtonSet[3])
        this->drawSetButtons(3);
    this->window.clear();
    this->window.draw(sprite);
    ImGui::SFML::Render(this->window);
    this->window.display();

}

uint8_t GUIClass::getControllerState() {

    const int dead_zone = 60;

    uint8_t temp = 0;
    sf::Joystick::update();
    if (sf::Joystick::isConnected(0)) {
         temp |= (sf::Joystick::isButtonPressed(0, this->controller->getButton(0))) << 0;    //  A pressed
         temp |= (sf::Joystick::isButtonPressed(0, this->controller->getButton(1))) << 1;    //  B pressed
         temp |= (sf::Joystick::isButtonPressed(0, this->controller->getButton(2))) << 2;    //  select pressed
         temp |= (sf::Joystick::isButtonPressed(0, this->controller->getButton(3))) << 3;    //  start pressed
         temp |= (sf::Joystick::getAxisPosition(0,sf::Joystick::Axis::PovY) > dead_zone) << 4;    //  up pressed dpad
         temp |= (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y) < -dead_zone) << 4;     //  up pressed left stick
         temp |= (sf::Joystick::getAxisPosition(0,sf::Joystick::Axis::PovY) < -dead_zone) << 5;   //  down pressed dpad
         temp |= (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y) > dead_zone) << 5;   //  down pressed left stick
         temp |= (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX) < -dead_zone) << 6;  //  left pressed dpad
         temp |= (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) < -dead_zone) << 6;     //  left pressed left stick
         temp |= (sf::Joystick::getAxisPosition(0,sf::Joystick::Axis::PovX) > dead_zone) << 7;    //  right pressed dpad
         temp |= (sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) > dead_zone) << 7;      //  right pressed left stick
    }
    else {
        temp |= (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) << 0;    //  A pressed
        temp |= (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) << 1;    //  B pressed
        temp |= (sf::Keyboard::isKeyPressed(sf::Keyboard::T)) << 2;    //  select pressed
        temp |= (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) << 3;    //  start pressed
        temp |= (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) << 4;    //  up pressed
        temp |= (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) << 5;    //  down pressed
        temp |= (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) << 6;    //  left pressed
        temp |= (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) << 7;    //  right pressed
    }
    return temp;

}

bool GUIClass::isLoaded() {

    if (this->cartridge != nullptr)
        return this->cartridge->loaded();
    return false;

}

void GUIClass::updateFrame(sf::Image& pixels) {

    //  need to adjust last parameter(pitch) based on PPU information(dimensions of the screen to be rendered)
    uint32_t x, y;
    std::tie(x, y) = this->PPU->getResolution();
    //this->gameTexture.update((uint8_t*)pixels, x, y, 0, 0);
    this->gameTexture.update(pixels);

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
    if (ImGui::BeginMenu("Settings")) {
        if (ImGui::MenuItem("Set Controller Binds" ) && sf::Joystick::isConnected(0)) {
            showControllerDialog = true;

        }  

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();

}

void::GUIClass::drawControllerDialog() {
  
    ImGui::Begin("Controller Bindings", &showControllerDialog);  
    ImGui::Text("'A' is currently button: %d", this->controller->getButton(0));    
    if (ImGui::Button("Rebind A")) { showButtonSet[0] = true; }
    ImGui::Text("'B' is currently button: %d", this->controller->getButton(1));
    if (ImGui::Button("Rebind B")) { showButtonSet[1] = true; }
    ImGui::Text("'Select' is currently button: %d", this->controller->getButton(2));
    if (ImGui::Button("Rebind Select")) { showButtonSet[2] = true; }
    ImGui::Text("'Start' is currently button: %d", this->controller->getButton(3));
    if (ImGui::Button("Rebind Start")) { showButtonSet[3] = true; }
    if (ImGui::Button("Save To ini")) { this->controller->setIni(); }
            
    ImGui::End();          
        //a b select start   
}

void::GUIClass::drawSetButtons(int b) {
    switch (b){
    case 0:
        ImGui::Begin("A Binding", &showButtonSet[0]);
        ImGui::Text("'A' is currently button: %d", this->controller->getButton(0));
        ImGui::Text("Press new 'A' buton...");
        for (int i = 0; i < sf::Joystick::getButtonCount(0); i++) {
            if (sf::Joystick::isButtonPressed(0, i)) {
                this->controller->setButton(0, i);
                i = sf::Joystick::getButtonCount(0);
            }
        }
        ImGui::End();
        break;
    case 1:
        ImGui::Begin("B Binding", &showButtonSet[1]);
        ImGui::Text("'B' is currently button: %d", this->controller->getButton(1));
        ImGui::Text("Press new 'B' buton...");
        for (int i = 0; i < sf::Joystick::getButtonCount(0); i++) {
            if (sf::Joystick::isButtonPressed(0, i)) {
                this->controller->setButton(1, i);
                i = sf::Joystick::getButtonCount(0);
            }
        }
        ImGui::End();
        break;
    case 2:
        ImGui::Begin("Select Binding", &showButtonSet[2]);
        ImGui::Text("'Select' is currently button: %d", this->controller->getButton(2));
        ImGui::Text("Press new 'Select' buton...");
        for (int i = 0; i < sf::Joystick::getButtonCount(0); i++) {
            if (sf::Joystick::isButtonPressed(0, i)) {
                this->controller->setButton(2, i);
                i = sf::Joystick::getButtonCount(0);
            }
        }
        ImGui::End();
        break;
    case 3:
        ImGui::Begin("Start Binding", &showButtonSet[3]);
        ImGui::Text("'Start' is currently button: %d", this->controller->getButton(3));
        ImGui::Text("Press new 'Start' buton...");
        for (int i = 0; i < sf::Joystick::getButtonCount(0); i++) {
            if (sf::Joystick::isButtonPressed(0, i)) {
                this->controller->setButton(3, i);
                i = sf::Joystick::getButtonCount(0);
            }
        }
        ImGui::End();
        break;
    default:
        break;
    }

}

void GUIClass::drawSelectDebug() {

    //  creates the checkbox for each stored MemoryEditor we have currently
    //  this will grow on the fly if we decide to allow the complete removal/addition of sections by the user
    ImGui::Begin("Debug Window Select", &showSelectDebug);
    for (uint32_t i = 0; i < this->cart_hex_windows.size(); i++)
        ImGui::Checkbox(this->cart_hex_windows.at(i).hex_name.c_str(), &this->cart_hex_windows.at(i).isShown);
    for (uint32_t i = 0; i < this->cpu_hex_windows.size(); i++)
        ImGui::Checkbox(this->cpu_hex_windows.at(i).hex_name.c_str(), &this->cpu_hex_windows.at(i).isShown);
    for (uint32_t i = 0; i < this->ppu_hex_windows.size(); i++)
        ImGui::Checkbox(this->ppu_hex_windows.at(i).hex_name.c_str(), &this->ppu_hex_windows.at(i).isShown);
    ImGui::End();

}

void GUIClass::drawDebug() {

    ImGui::Begin("Information");
    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Loaded File: %s", this->loadedFile.c_str());
    if (this->isLoaded()) {

        uint32_t x, y;
        std::tie(x, y) = this->PPU->getResolution();
        ImGui::Text("Resolution: %dx%d", x, y);

        sf::Vector2u res = this->gameTexture.getSize();
        ImGui::Text("Tex Res: %dx%d", res.x, res.y);

    }
    ImGui::End();

    //  here we use the function pointers we stored before to grab the data we wish to view, if it is available
    for (uint32_t i = 0; i < this->cart_hex_windows.size(); i++)
        if (this->cart_hex_windows.at(i).isShown && (this->cartridge->*this->cart_hex_windows.at(i).getDataFunc)() != nullptr)
            this->cart_hex_windows.at(i).hex_view->DrawWindow(this->cart_hex_windows.at(i).hex_name.c_str(), (this->cartridge->*this->cart_hex_windows.at(i).getDataFunc)(), (this->cartridge->*this->cart_hex_windows.at(i).getSizeFunc)());
    for (uint32_t i = 0; i < this->cpu_hex_windows.size(); i++)
        //  CPU sections will always be available, therefore we do not need to check if these sections actually exist or not
        if (this->cpu_hex_windows.at(i).isShown)
            this->cpu_hex_windows.at(i).hex_view->DrawWindow(this->cpu_hex_windows.at(i).hex_name.c_str(), (this->CPU->*this->cpu_hex_windows.at(i).getDataFunc)(), (this->CPU->*this->cpu_hex_windows.at(i).getSizeFunc)());
    for (uint32_t i = 0; i < this->ppu_hex_windows.size(); i++)
        if (this->ppu_hex_windows.at(i).isShown)
            this->ppu_hex_windows.at(i).hex_view->DrawWindow(this->ppu_hex_windows.at(i).hex_name.c_str(), (this->PPU->*this->ppu_hex_windows.at(i).getDataFunc)(), (this->PPU->*this->ppu_hex_windows.at(i).getSizeFunc)());

}

void GUIClass::drawFileDialog() {

    //  "Load ROM" buttonw as pressed, therefore we need to display the file directory to choose what rom file to load

    filebrowser.Display();

    if (filebrowser.HasSelected()) {

        this->loadedFile = filebrowser.GetSelected().string();

        if (loadedFile.find(".nes") != std::string::npos && ((loadedFile.find_last_of(".") - loadedFile.find_last_of("\\")) > 1)) {

            //  grab the file path name and load the new ROM in
            try {
                this->cartridge->storeMapper(selectMapper(this->cartridge->load(this->loadedFile)));

                //  store just the file name itself
                this->loadedFile = loadedFile.substr(loadedFile.find_last_of("\\") + 1);

                //  reset the CPU to a known state to start the new ROM
                //  emulation requires the cartridge ROM be loaded before the CPU can be started/restarted
                //  because the CPU must read a specific section of the ROM(0xFFFC and 0xFFFD) to find where to begin execution
                this->CPU->reset();

                //  we would need to reset all components here that require it(APU and PPU specifically)

                //  check if we have the PPU loaded, if not, we can ignore this
                if (this->PPU) {

                    this->PPU->reset();
                    uint32_t x, y;
                    std::tie(x, y) = this->PPU->getResolution();
                    this->gameTexture.create(x, y);

                }
                // We've succesfully loaded a ROM, disable error window if user hasn't closed it
                showMapperError = false;

            }
           
            
            catch (MapperException e)
            {
                // Unsupported Mapper - show error window
                showMapperError = true;

                // Log the error with full filepath
                errorLog.Write("Unsupported Mapper. Cannot load file: " + this->loadedFile);

                //  store just the file name itself - to be used in error message
                this->loadedFile = loadedFile.substr(loadedFile.find_last_of("\\") + 1);

            }

        }
        else
            this->loadedFile = "";

        showFileDialog = false;

    }

}

void GUIClass::drawErrorWindow() {

    // Error caused by trying to load a ROM that uses an unsupported mapper
    ImGui::Begin("Unsupported Mapper Error", &showMapperError);
    ImGui::Text("Cannot load file: %s", this->loadedFile.c_str());
    ImGui::Text("File uses an unsupported Mapper.");
    ImGui::Text("Please load a different ROM.");
    ImGui::End();
}