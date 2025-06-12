#pragma once

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_video.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include <SDL3/SDL.h>
#include <format>
#include <print>
#include <iostream>
#include <expected>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif


namespace StellatusEditor {
 class Editor {
 public:
 Editor();
 ~Editor() noexcept;
 void MainLoop();

 private:
 bool InitWindow();
 bool InitRHI();
 bool Loop(bool done);
 void Destory() noexcept;
 private:
 SDL_Window* window;
 SDL_GLContext glContext;
 ImGuiContext* ctx;
 ImGuiIO* io;
 const char * glslVersion;
 private: 
 bool bCanShutDown {false};
    };
}