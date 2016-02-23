// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <memory>

#include "common/emu_window.h"
#include "common/make_unique.h"
#include "common/logging/log.h"

#include "core/core.h"
#include "core/settings.h"

#include "video_core/pica.h"
#include "video_core/renderer_base.h"
#include "video_core/video_core.h"
#ifdef VKENABLED
#include "video_core/renderer_vulkan/renderer_vulkan.h"
#endif
#include "video_core/renderer_opengl/renderer_opengl.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Video Core namespace

namespace VideoCore {

EmuWindow*                    g_emu_window = nullptr; ///< Frontend emulator window
std::unique_ptr<RendererBase> g_renderer;             ///< Renderer plugin

std::atomic<bool> g_hw_renderer_enabled;
std::atomic<bool> g_shader_jit_enabled;

/// Initialize the video core
void Init(EmuWindow* emu_window) {
    Pica::Init();

    g_emu_window = emu_window;
#ifdef VKENABLED
    if (emu_window->VulkanSupported()){
        g_renderer = Common::make_unique<RendererVulkan>();
    }else{
        g_renderer = Common::make_unique<RendererOpenGL>();
    }
#else
    g_renderer = Common::make_unique<RendererOpenGL>();
#endif
    g_renderer->SetWindow(g_emu_window);
    g_renderer->Init();

    LOG_DEBUG(Render, "initialized OK");
}

/// Shutdown the video core
void Shutdown() {
    Pica::Shutdown();

    g_renderer.reset();

    LOG_DEBUG(Render, "shutdown OK");
}

} // namespace
