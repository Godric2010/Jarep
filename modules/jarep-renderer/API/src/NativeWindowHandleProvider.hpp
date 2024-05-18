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
            windowHandle = handle;
            windowWidth = width;
            windowHeight = height;
            windowSystem = system;
        }

        virtual ~NativeWindowHandleProvider() = default;

        [[nodiscard]] void* getNativeWindowHandle() const { return windowHandle; }

        [[nodiscard]] int getWindowWidth() const { return windowWidth; }

        [[nodiscard]] int getWindowHeight() const { return windowHeight; }

        [[nodiscard]] WindowSystem getWindowSystem() const { return windowSystem; }

    private:
        void* windowHandle;
        int windowWidth;
        int windowHeight;
        WindowSystem windowSystem;
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
			displayConnection = connection;
			xlibWindow = handle;
		}

		Display *getDisplayConnection() { return displayConnection; }

		[[nodiscard]] Window getXcbWindow() const { return xlibWindow; }

	private:
		Display *displayConnection;
		Window xlibWindow;
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

		wl_display *getDisplay() { return display; }

		wl_surface *getWindowHandle() { return surface; }

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
