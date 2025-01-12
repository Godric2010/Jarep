# API Documentation

---
## class JAREP::Window::IWindowHandle

**Description**:<br>Wrapper class around the various window handles. Currently, supports Windows (WinWindowHandle) and Linux X11 (X11WindowHandle). Cast into the respected window handles to use this. Cast options depending on the OS at usage.

---

---

---
## class JAREP::Window::IWindowManager

**Description**:<br>Provides core functionality to the library.

---

### Public Functions

### ` ~IWindowManager()`

**Description**:<br>*Destructor of the Window Management.*


### `bool Initialize(WindowSettings display_settings)`

**Description**:<br>*Initialize a new window.*

*Returns: `bool`*<br>*&nbsp;&nbsp;&nbsp;&nbsp;True if window was created successful.*

*`WindowSettings display_settings`*<br>&nbsp;&nbsp;&nbsp;&nbsp;*Settings of this window.* 


### `void SetWindowSettings(WindowSettings display_settings)`

**Description**:<br>*Set new display settings.*

*`WindowSettings display_settings`*<br>&nbsp;&nbsp;&nbsp;&nbsp;*The new settings that will be applied to the window.* 


### `std::expected< void, std::string > RegisterForWindowUpdate(WindowUpdatedCallback callback)`

**Description**:<br>*Register for the window update event.*

*Returns: `std::expected< void, std::string >`*<br>*&nbsp;&nbsp;&nbsp;&nbsp;*

*`WindowUpdatedCallback callback`*<br>&nbsp;&nbsp;&nbsp;&nbsp;*callback function to execute on window update* 


### `void PollEvents()`

**Description**:<br>*Poll the events from this window if its focussed.*


### `bool ShouldClose()`

**Description**:<br>*Evaluates if the window has been called to close.*

*Returns: `bool`*<br>*&nbsp;&nbsp;&nbsp;&nbsp;True if the window is about to be destroyed, False if not.*


### `void DestroyWindow()`

**Description**:<br>*Destroy the window and remove it from memory.*


### `int32_t GetWindowWidth()`

**Description**:<br>*Get the current width of the window.*

*Returns: `int32_t`*<br>*&nbsp;&nbsp;&nbsp;&nbsp;The window width in pixels.*


### `int32_t GetWindowHeight()`

**Description**:<br>*Get the current height of the window.*

*Returns: `int32_t`*<br>*&nbsp;&nbsp;&nbsp;&nbsp;The window height in pixels.*


### `std::optional< IWindowHandle > GetNativeWindowHandle()`

**Description**:<br>*Get the window handle of this window.*

*Returns: `std::optional< IWindowHandle >`*<br>*&nbsp;&nbsp;&nbsp;&nbsp;Can be null-opt if platform is invalid. Returns valid IWindowHandle if platform is supported. Cast to platform handle for further usage.*


---

---
## struct JAREP::Window::WindowSettings

**Description**:<br>Contains all settings of the current window.

---

### Public Fields

`std::string windowTitle `<br>&nbsp;&nbsp;**Description:** The title of the window. 

`int8_t displayIndex = 0`<br>&nbsp;&nbsp;**Description:** The index of the display the window is shown at. 0 by default, which brings the window to the main screen. 

`int32_t displayWidth = 0`<br>&nbsp;&nbsp;**Description:** The pixel resolution width of this window. 

`int32_t displayHeight = 0`<br>&nbsp;&nbsp;**Description:** The pixel resolution height of this window. 

`int32_t displayRefreshRate = 0`<br>&nbsp;&nbsp;**Description:** The refresh rate of this window in hertz. 

`DisplayMode displayMode = DisplayMode::BorderedWindow`<br>&nbsp;&nbsp;**Description:** The mode the display has to be set to. 

---

