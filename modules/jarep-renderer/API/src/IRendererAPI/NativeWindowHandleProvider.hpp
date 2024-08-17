//
// Created by sebastian on 09.11.23.
//

#ifndef NATIVEWINDOWHANDLEPROVIDER_HPP
#define NATIVEWINDOWHANDLEPROVIDER_HPP

namespace Graphics
{
    enum WindowSystem
    {
        Win32,
        Cocoa,
        X11,
        Wayland
    };

    class NativeWindowHandleProvider
    {
    public:
        NativeWindowHandleProvider(void* handle, const int width, const int height, const WindowSystem system)
        {
			m_windowHandle = handle;
			m_windowWidth = width;
			m_windowHeight = height;
			m_windowSystem = system;
        }

        virtual ~NativeWindowHandleProvider() = default;

        [[nodiscard]] void* GetNativeWindowHandle() const { return m_windowHandle; }

        [[nodiscard]] int GetWindowWidth() const { return m_windowWidth; }

        [[nodiscard]] int GetWindowHeight() const { return m_windowHeight; }

        [[nodiscard]] WindowSystem GetWindowSystem() const { return m_windowSystem; }

    private:
        void* m_windowHandle;
        int m_windowWidth;
        int m_windowHeight;
        WindowSystem m_windowSystem;
    };

    class XlibWindowHandleProvider;

    class WaylandWindowHandleProvider;

    class WindowsWindowHandleProvider;
}

#if defined(__linux__)

#include <X11/Xlib.h>

class Graphics::XlibWindowHandleProvider : public Graphics::NativeWindowHandleProvider {
	public:
		XlibWindowHandleProvider(Window handle, Display *connection, const int width, const int height,
		                         const WindowSystem system)
				: Graphics::NativeWindowHandleProvider(reinterpret_cast<void *>(handle), width, height, system) {
		    m_displayConnection = connection;
		    m_xlibWindow = handle;
		}

		Display * GetDisplayConnection() { return m_displayConnection; }

		[[nodiscard]] Window GetXcbWindow() const { return m_xlibWindow; }

	private:
		Display * m_displayConnection;
		Window m_xlibWindow;
};

#include <wayland-egl.h>

class Graphics::WaylandWindowHandleProvider : public Graphics::NativeWindowHandleProvider {
	public:
		WaylandWindowHandleProvider(wl_surface *handle, wl_display *wl_display, const int width, const int height,
		                            const WindowSystem system)
				: Graphics::NativeWindowHandleProvider(reinterpret_cast<void *>(handle), width, height, system) {

			surface = handle;
			display = wl_display;
		}

		wl_display * GetDisplay() { return display; }

		wl_surface * GetWindowHandle() { return surface; }

	private:
		wl_surface *surface;
		wl_display *display;
};

#endif

#if defined (_WIN32)
#include <windows.h>

class Graphics::WindowsWindowHandleProvider final : public NativeWindowHandleProvider
{
public:
    WindowsWindowHandleProvider(HWND handle, HINSTANCE hInst, const int width, const int height,
                                const WindowSystem system) : NativeWindowHandleProvider(
        reinterpret_cast<void*>(handle), width, height, system)
    {
        hInstance = hInst;
        window = handle;
    }

    HINSTANCE getHIstance() const { return hInstance; }
    HWND getWindowHandle() const { return window; }

private:
    HINSTANCE hInstance;
    HWND window;
};

#endif
#endif //NATIVEWINDOWHANDLEPROVIDER_HPP
