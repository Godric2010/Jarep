# API Documentation

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


### `void PollEvents()`

**Description**:<br>*Poll the events from this window if its focussed.*


### `bool ShouldClose()`

**Description**:<br>*Evaluates if the window has been called to close True if the window is about to be destroyed, False if not.*

*Returns: `bool`*<br>*&nbsp;&nbsp;&nbsp;&nbsp;True if the window is about to be destroyed, False if not.*


### `void DestroyWindow()`

**Description**:<br>*Destroy the window and remove it from memory.*


---

---
## struct JAREP::Window::WindowSettings

**Description**:<br>Contains all settings of the current window.

---

### Public Fields

`std::string windowTitle `<br>&nbsp;&nbsp;**Description:** The title of the window. 

`int displayIndex = 0`<br>&nbsp;&nbsp;**Description:** The index of the display the window is shown at. 0 by default, which brings the window to the main screen. 

`int displayWidth = 0`<br>&nbsp;&nbsp;**Description:** The pixel resolution width of this window. 

`int displayHeight = 0`<br>&nbsp;&nbsp;**Description:** The pixel resolution height of this window. 

`int displayRefreshRate = 0`<br>&nbsp;&nbsp;**Description:** The refresh rate of this window in hertz. 

---

