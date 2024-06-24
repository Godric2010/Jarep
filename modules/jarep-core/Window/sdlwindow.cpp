//
// Created by Sebastian Borsch on 13.10.23.
//

#include "sdlwindow.hpp"

#if defined(__linux__)
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#endif

namespace Core::Window {
	SdlWindow::SdlWindow() {
		window = nullptr;
		displayModes = std::vector<DisplayOpts>();
		windowFlags = 0;
		m_currentWidth = 0;
		m_currentHeight = 0;
		activeOptIndex = 0;
		isDirty = false;
	}

	SdlWindow::~SdlWindow() {
		window = nullptr;
	}

	void SdlWindow::Init(const int windowWidth, const int windowHeight) {
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			return;
		}

		m_currentWidth = windowWidth;
		m_currentHeight = windowHeight;

#if defined(__APPLE__)
		windowFlags = SDL_WINDOW_METAL;
#else
		windowFlags = SDL_WINDOW_VULKAN;
#endif


		windowFlags |= SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

		displayModes = getAvailableDisplayOpts();


		window = SDL_CreateWindow(
				"J.A.R.E.P",
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				windowWidth, m_currentHeight,
				windowFlags
		);

		std::vector<const char*> graphicsExtensions;
#if defined(__APPLE__)
		graphicsExtensions = std::vector<const char*>();
#else
		graphicsExtensions = getVulkanWindowExtensionsCStr();
#endif
		renderer = std::make_shared<Graphics::JarRenderer>(graphicsExtensions);

		const auto nativeWindowHandleProvider = getNativeWindowHandle(windowWidth, windowHeight);
		if (!nativeWindowHandleProvider.has_value()) throw std::exception();
		renderer->Initialize(nativeWindowHandleProvider.value(), windowWidth, windowHeight);
		if (window == nullptr) {
			return;
		}
	}

	void SdlWindow::Update() {
		bool running = true;
		SDL_Event event;

		int counter = 0;
		while (running) {
			counter++;
			m_resizeOccurred = false;

//			if(counter > 10) running = false;
			if (isDirty) {
				auto newDisplayMode = getDisplayModeFromOpts();
				if (newDisplayMode.has_value()) {
					const auto ndp = newDisplayMode.value();
					SDL_SetWindowDisplayMode(window, &ndp);
				}
				isDirty = false;
			}

			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					running = false;
				} else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					m_pendingResize = std::make_pair(event.window.data1, event.window.data2);
					m_lastResizeTime = std::chrono::steady_clock::now();
					continue;
				} else if (event.type == SDL_KEYDOWN) {
					HandleKeyDownEvent(event);
				}
			}

			if (m_pendingResize &&
			    std::chrono::steady_clock::now() - m_lastResizeTime > std::chrono::milliseconds(100)) {
				m_currentWidth = m_pendingResize->first;
				m_currentHeight = m_pendingResize->second;
				renderer->ResizeSurface(m_currentWidth, m_currentHeight);

				m_pendingResize = std::nullopt;
				m_resizeOccurred = true;
			}


			if (!m_resizeOccurred && !m_pendingResize)
				renderer->Render();
		}
	}

	void SdlWindow::Shutdown() {
		renderer->Shutdown();
		SDL_Quit();
	}

	std::shared_ptr<Graphics::JarRenderer> SdlWindow::getRenderer() const {
		return renderer;
	}


	void SdlWindow::HandleKeyDownEvent(const SDL_Event& event) {
		switch (event.key.keysym.sym) {
			case SDLK_f:
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
				break;
			case SDLK_b:
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
				break;
			case SDLK_d:
				SDL_SetWindowFullscreen(window, 0);
				SDL_SetWindowSize(window, m_currentWidth, m_currentHeight);
				break;
		}
	}

	std::vector<DisplayOpts> SdlWindow::GetAvailableDisplayOpts() {
		return displayModes;
	}

	void SdlWindow::SetDisplayOpts(int displayIndex, int resolutionIndex, int refreshRateIndex) {
		for (int optIndex = 0; optIndex < displayModes.size(); ++optIndex) {
			if (displayModes[optIndex].Index != displayIndex)continue;
			displayModes[optIndex].SelectResolutionAndRefreshRate(resolutionIndex, refreshRateIndex);
			activeOptIndex = optIndex;
			isDirty = true;
			return;
		}
	}


	std::vector<DisplayOpts> SdlWindow::getAvailableDisplayOpts() {
		auto displaysWithOptions = std::vector<DisplayOpts>();

		int displaysFound = SDL_GetNumVideoDisplays();
		for (int displayIndex = 0; displayIndex < displaysFound; ++displayIndex) {
			DisplayOpts displayOpts;
			displayOpts.Index = displayIndex;
			const char* name = SDL_GetDisplayName(displayIndex);
			displayOpts.Name = name ? name : "Unknown";

			std::set<int> uniqueRefreshRates;
			std::set<std::pair<int, int>> uniqueResolutions;

			int modesFound = SDL_GetNumDisplayModes(displayIndex);
			for (int displayMode = 0; displayMode < modesFound; ++displayMode) {
				SDL_DisplayMode mode;
				if (SDL_GetDisplayMode(displayIndex, displayMode, &mode) != 0) {
					return {};
				}

				uniqueResolutions.insert({mode.w, mode.h});
				uniqueRefreshRates.insert(mode.refresh_rate);
			}

			displayOpts.Resolutions = std::vector(uniqueResolutions.begin(),
			                                      uniqueResolutions.end());
			displayOpts.RefreshRates = std::vector(uniqueRefreshRates.begin(), uniqueRefreshRates.end());
			displaysWithOptions.push_back(displayOpts);
		}
		return displaysWithOptions;
	}

	std::optional<SDL_DisplayMode> SdlWindow::getDisplayModeFromOpts() {
		int displayIndex = displayModes[activeOptIndex].Index;
		int targetRefreshRate = displayModes[activeOptIndex].GetCurrentRefreshRate();
		auto targetResolution = displayModes[activeOptIndex].GetCurrentResolution();

		int modesFound = SDL_GetNumDisplayModes(displayIndex);
		for (int modeIndex = 0; modeIndex < modesFound; ++modeIndex) {
			SDL_DisplayMode mode;
			if (SDL_GetDisplayMode(displayIndex, modeIndex, &mode) != 0) continue;

			if (mode.refresh_rate == targetRefreshRate && mode.w == targetResolution.first &&
			    mode.h == targetResolution.second) {
				return std::make_optional(mode);
			}
		}
		return std::nullopt;
	}

	std::optional<Graphics::NativeWindowHandleProvider*>
	SdlWindow::getNativeWindowHandle(int sizeWidth, int sizeHeight) const {
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		if (SDL_GetWindowWMInfo(window, &wmInfo) != SDL_TRUE) {
			return std::nullopt;
		}
#if defined(_WIN32)
		auto nativeWindowHandleProvider = new Graphics::WindowsWindowHandleProvider(
			wmInfo.info.win.window, wmInfo.info.win.hinstance, sizeWidth, sizeHeight, Graphics::Win32);
		return std::make_optional(nativeWindowHandleProvider);
#elif defined(__APPLE__) && defined(__MACH__)
		auto nativeWindowHandleProvider = new Graphics::NativeWindowHandleProvider(
				reinterpret_cast<void*>( wmInfo.info.cocoa.window), sizeWidth, sizeHeight, Graphics::Cocoa);
		return std::make_optional(nativeWindowHandleProvider);
#elif defined(__linux__) || defined(__unix__)
		// X11
#if defined(SDL_VIDEO_DRIVER_X11)
		auto xlibHandleProvider = new Graphics::XlibWindowHandleProvider(wmInfo.info.x11.window,
																		 wmInfo.info.x11.display,
																		 sizeWidth, sizeHeight, Graphics::X11);


		return std::make_optional(xlibHandleProvider);
		// Wayland
#elif defined(SDL_VIDEO_DRIVER_WAYLAND)
		auto wlHandleProvider = Graphics::WaylandWindowHandleProvider(wmInfo.info.wl.m_surface, wmInfo.info.wl.display,
																	  sizeWidth, sizeWidth, Graphics::Wayland);
		return std::make_optional(wlHandleProvider);
#endif

#else
		return std::nullopt; // Unsupported platform
#endif
	}

	std::vector<const char*> SdlWindow::getVulkanWindowExtensionsCStr() const {
		unsigned int extensionCount = 0;
		if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr)) {
			throw std::runtime_error("Could not get the number of vulkan m_extensions");
		}

		std::vector<const char*> sdlExtensions(extensionCount);
		if (!SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, sdlExtensions.data())) {
			throw std::runtime_error("Could not get the vulkan instance m_extensions.");
		}
		return sdlExtensions;
	}
}
