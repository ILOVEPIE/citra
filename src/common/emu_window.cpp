// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include "emu_window.h"
#include "video_core/video_core.h"

void EmuWindow::KeyPressed(KeyMap::HostDeviceKey key) {
    pad_state.hex |= KeyMap::GetPadKey(key).hex;
}

void EmuWindow::KeyReleased(KeyMap::HostDeviceKey key) {
    pad_state.hex &= ~KeyMap::GetPadKey(key).hex;
}

/**
 * Check if the given x/y coordinates are within the touchpad specified by the framebuffer layout
 * @param layout FramebufferLayout object describing the framebuffer size and screen positions
 * @param framebuffer_x Framebuffer x-coordinate to check
 * @param framebuffer_y Framebuffer y-coordinate to check
 * @return True if the coordinates are within the touchpad, otherwise false
 */
static bool IsWithinTouchscreen(const EmuWindow::FramebufferLayout& layout, unsigned framebuffer_x,
                                unsigned framebuffer_y) {
    return (framebuffer_y >= layout.bottom_screen.top    &&
            framebuffer_y <  layout.bottom_screen.bottom &&
            framebuffer_x >= layout.bottom_screen.left   &&
            framebuffer_x <  layout.bottom_screen.right);
}

void EmuWindow::TouchPressed(const FramebufferLayout& layout, unsigned framebuffer_x,
                             unsigned framebuffer_y) {
    if (IsWithinTouchscreen(layout, framebuffer_x, framebuffer_y)) {
        touch_x = VideoCore::kScreenBottomWidth * (framebuffer_x - layout.bottom_screen.left) /
            (layout.bottom_screen.right - layout.bottom_screen.left);
        touch_y = VideoCore::kScreenBottomHeight * (framebuffer_y - layout.bottom_screen.top) /
            (layout.bottom_screen.bottom - layout.bottom_screen.top);
        touch_pressed = true;
        pad_state.touch = 1;
    }
}

void EmuWindow::TouchReleased() {
    touch_pressed = false;
    touch_x = 0;
    touch_y = 0;
    pad_state.touch = 0;
}

void EmuWindow::TouchMoved(const FramebufferLayout& layout, unsigned framebuffer_x,
                            unsigned framebuffer_y) {
    if (touch_pressed) {
        if (IsWithinTouchscreen(layout, framebuffer_x, framebuffer_y))
            TouchPressed(layout, framebuffer_x, framebuffer_y);
        else
            TouchReleased();
    }
}

EmuWindow::FramebufferLayout EmuWindow::FramebufferLayout::DefaultScreenLayout(unsigned width,
    unsigned height) {

    ASSERT(width > 0);
    ASSERT(height > 0);

    EmuWindow::FramebufferLayout res = { width, height, {}, {} };

    float window_aspect_ratio = static_cast<float>(height) / width;
    float emulation_aspect_ratio = static_cast<float>(VideoCore::kScreenTopHeight * 2) /
        VideoCore::kScreenTopWidth;

    if (window_aspect_ratio > emulation_aspect_ratio) {
        // Window is narrower than the emulation content => apply borders to the top and bottom
        int viewport_height = static_cast<int>(std::round(emulation_aspect_ratio * width));

        res.top_screen.left = 0;
        res.top_screen.right = res.top_screen.left + width;
        res.top_screen.top = (height - viewport_height) / 2;
        res.top_screen.bottom = res.top_screen.top + viewport_height / 2;

        int bottom_width = static_cast<int>((static_cast<float>(VideoCore::kScreenBottomWidth) /
            VideoCore::kScreenTopWidth) * (res.top_screen.right - res.top_screen.left));
        int bottom_border = ((res.top_screen.right - res.top_screen.left) - bottom_width) / 2;

        res.bottom_screen.left = bottom_border;
        res.bottom_screen.right = res.bottom_screen.left + bottom_width;
        res.bottom_screen.top = res.top_screen.bottom;
        res.bottom_screen.bottom = res.bottom_screen.top + viewport_height / 2;
    } else {
        // Otherwise, apply borders to the left and right sides of the window.
        int viewport_width = static_cast<int>(std::round(height / emulation_aspect_ratio));

        res.top_screen.left = (width - viewport_width) / 2;
        res.top_screen.right = res.top_screen.left + viewport_width;
        res.top_screen.top = 0;
        res.top_screen.bottom = res.top_screen.top + height / 2;

        int bottom_width = static_cast<int>((static_cast<float>(VideoCore::kScreenBottomWidth) /
            VideoCore::kScreenTopWidth) * (res.top_screen.right - res.top_screen.left));
        int bottom_border = ((res.top_screen.right - res.top_screen.left) - bottom_width) / 2;

        res.bottom_screen.left = res.top_screen.left + bottom_border;
        res.bottom_screen.right = res.bottom_screen.left + bottom_width;
        res.bottom_screen.top = res.top_screen.bottom;
        res.bottom_screen.bottom = res.bottom_screen.top + height / 2;
    }

    return res;
}
