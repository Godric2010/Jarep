//
// Created by sebastian on 1/12/2025.
//

#pragma once

namespace JAREP::Window {
    /**
     * Wrapper class around the various window handles.
     * Currently, supports Windows (WinWindowHandle) and Linux X11 (X11WindowHandle).
     * Cast into the respected window handles to use this. Cast options depending on the OS at usage.
     */
    class IWindowHandle {
    };

    /**
     * Window Handle for windows platforms. Contains the HWND handle as well as the HINSTANCE reference.
     */
    class WinWindowHandle;

    /**
     * X11 handle for linux environments. Contains the X11 Window as well as the X11 Display connection.
     */
    class X11WindowHandle;

#if defined (_WIN32)
#include <windows.h>

    class WinWindowHandle : public IWindowHandle {
    public:
        WinWindowHandle(HWND handle, HINSTANCE instance): handle(handle), instance(instance) {
        }

        ~WinWindowHandle() = default;

        [[nodiscard]] HWND getHandle() const { return handle; }
        [[nodiscard]] HINSTANCE getInstance() const { return instance; }

    private:
        HWND handle;
        HINSTANCE instance;
    };

#endif

#if defined (__linux__)

#include <X11/Xlib.h>

    class X11WindowHandle : public IWindowHandle {
    public:
        X11WindowHandle(Window handle, Display* connection) : m_handle(handle), m_connection(connection) {}
        ~X11WindowHandle() = default;

        [[nodiscard]] Window getWindow() const { return m_handle; }

        [[nodiscard]] Display* getDisplay() const { return m_connection; }

    private:
        Window m_handle;
        Display* m_connection;

    };
#endif
}
