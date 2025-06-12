#include "Editor.h"

StellatusEditor::Editor::Editor(){
 if (!InitWindow()) {
 Destory();
 return;
    }
 std::cout << "InitWindow\n";
 if (!InitRHI()) {
 Destory();
 return;
    }
 std::cout << "InitRHI\n";
 SDL_GL_MakeCurrent(window, glContext);
 SDL_GL_SetSwapInterval(1); // Enable vsync
 SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
 SDL_ShowWindow(window);
 IMGUI_CHECKVERSION();
    // 设置imgui上下文
 this->ctx = ImGui::CreateContext();
 ImGui::SetCurrentContext(ctx);
 
    //  初始化io
 this->io = &ImGui::GetIO();
    // ImGuiIO& io = ImGui::GetIO(); (void)io;
    (*io).ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    (*io).ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls  
 
    // 设置主题
 ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // 初始化后端
    // Setup Platform/Renderer backends
 ImGui_ImplSDL3_InitForOpenGL(window, glContext);
 ImGui_ImplOpenGL3_Init(this->glslVersion);
 
 this->bCanShutDown = false;
}

StellatusEditor::Editor::~Editor() noexcept {
 Destory();
}

void StellatusEditor::Editor::MainLoop() {
 ImGui::SetCurrentContext(this->ctx);
 bool done = false;
 while(Loop(done));
}


bool StellatusEditor::Editor::InitWindow() {
 if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)){
 auto error = std::format("Error: SDL_Init(): {}", SDL_GetError());
 std::cout << error << "\n";
 return false;
    }
 

 SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | 
 SDL_WINDOW_RESIZABLE | 
 SDL_WINDOW_HIDDEN | 
 SDL_WINDOW_HIGH_PIXEL_DENSITY;

 this->window = SDL_CreateWindow("Stellatus Engine",  1280, 720, window_flags);
 if (window == nullptr){
 auto error = std::format("Error: SDL_CreateWindow(): {}", SDL_GetError());
 std::cout << error << "\n";
 return false;
    }
 return true;
}

bool StellatusEditor::Editor::InitRHI(){
    // Decide GL+GLSL versions
 #if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100 (WebGL 1.0)
 this->glslVersion = "#version 100";
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    #elif defined(IMGUI_IMPL_OPENGL_ES3)
        // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
 this->glslVersion = "#version 300 es";
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    #elif defined(__APPLE__)
        // GL 3.2 Core + GLSL 150
 this->glslVersion = "#version 150";
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    #else
        // GL 3.0 + GLSL 130
 this->glslVersion = "#version 130";
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    #endif
        // Create window with graphics context
 SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
 SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
 SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

 this->glContext = SDL_GL_CreateContext(window);
 if (glContext == nullptr){
 auto error = std::format("Error: SDL_GL_CreatContext(): {}", SDL_GetError());
 std::cout << error << "\n";
 return false;
    }
 return true;
}

bool StellatusEditor::Editor::Loop(bool done){
 bool show_demo_window = true;
 bool show_another_window = false;
 ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
 SDL_Event event;
 while (SDL_PollEvent(&event))
    {
 ImGui_ImplSDL3_ProcessEvent(&event);
 if (event.type == SDL_EVENT_QUIT)
 return false;
 if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
 return false;
    }

    // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
 if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
    {
 SDL_Delay(10);
        // continue
 return true;
    }

    // Start the Dear ImGui frame
 ImGui_ImplOpenGL3_NewFrame();
 ImGui_ImplSDL3_NewFrame();
 ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
 if (show_demo_window)
 ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
 static float f = 0.0f;
 static int counter = 0;

 ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

 ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
 ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
 ImGui::Checkbox("Another Window", &show_another_window);

 ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
 ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

 if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
 counter++;
 ImGui::SameLine();
 ImGui::Text("counter = %d", counter);

 ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / (*io).Framerate, (*io).Framerate);
 ImGui::End();
    }

    // 3. Show another simple window.
 if (show_another_window)
    {
 ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
 ImGui::Text("Hello from another window!");
 if (ImGui::Button("Close Me"))
 show_another_window = false;
 ImGui::End();
    }

    // Rendering
 ImGui::Render();
 glViewport(0, 0, (int)(*io).DisplaySize.x, (int)(*io).DisplaySize.y);
 glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
 glClear(GL_COLOR_BUFFER_BIT);
 ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
 SDL_GL_SwapWindow(window);
 return true;
}

void StellatusEditor::Editor::Destory() noexcept {
 if (!this->bCanShutDown){
 ImGui_ImplOpenGL3_Shutdown();
 ImGui_ImplSDL3_Shutdown();
    }
 if (this->ctx){
 ImGui::DestroyContext(ctx);
    }
 if (this->glContext){
 SDL_GL_DestroyContext(this->glContext);
    }
 if (this->window){
 SDL_DestroyWindow(this->window);
    }
 SDL_Quit();
}