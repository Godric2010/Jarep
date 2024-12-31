# API Documentation

---
## Class JAREP::Window::IWindowManager

**Description**:<br>Provides core functionality to the library.

---
### Public Functions

### `JAREP::Window::IWindowManager::IWindowManager()`

**Description**:<br>*Constructor of the Window Management.*

| **Type** | **Name** | **Description** |
| --- | --- | --- |
| void | *Return* |  |

### `virtual JAREP::Window::IWindowManager::~IWindowManager()`

**Description**:<br>*Destructor of the Window Management.*

| **Type** | **Name** | **Description** |
| --- | --- | --- |
| void | *Return* |  |

### `virtual bool JAREP::Window::IWindowManager::Initialize(WindowSettings display_settings)`

**Description**:<br>*Initialize a new window.*

| **Type** | **Name** | **Description** |
| --- | --- | --- |
| bool | *Return* | True if window was created successful. |
| WindowSettings | display_settings | Settings of this window. |

### `virtual void JAREP::Window::IWindowManager::PollEvents()`

**Description**:<br>*Poll the events from this window if its focussed.*

| **Type** | **Name** | **Description** |
| --- | --- | --- |
| void | *Return* |  |

### `virtual bool JAREP::Window::IWindowManager::ShouldClose()`


| **Type** | **Name** | **Description** |
| --- | --- | --- |
| bool | *Return* | True if the window is about to be destroyed, False if not. |

### `virtual void JAREP::Window::IWindowManager::DestroyWindow()`


| **Type** | **Name** | **Description** |
| --- | --- | --- |
| void | *Return* |  |

---
---
## Class JAREP::Window::WindowSettings

**Description**:<br>Contains all settings of the current window.

---
---
## Class JAREP

**Description**:<br>

---
---
## Class JAREP::Window

**Description**:<br>

---
