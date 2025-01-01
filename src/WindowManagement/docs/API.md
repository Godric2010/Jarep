# API Documentation

---
## Class JAREP::Window::IWindowManager

**Description**:<br>Provides core functionality to the library.

---
### Public Functions

### `virtual JAREP::Window::IWindowManager::~IWindowManager()=default`

**Description**:<br>*Destructor of the Window Management.*

| **Type** | **Name** | **Description** |
| --- | --- | --- |
| void | *Return* |  |

### `virtual bool JAREP::Window::IWindowManager::Initialize(WindowSettings display_settings)=0`

**Description**:<br>*Initialize a new window.*

| **Type** | **Name** | **Description** |
| --- | --- | --- |
| bool | *Return* | True if window was created successful. |
| WindowSettings | display_settings | Settings of this window. |

### `virtual void JAREP::Window::IWindowManager::PollEvents()=0`

**Description**:<br>*Poll the events from this window if its focussed.*

| **Type** | **Name** | **Description** |
| --- | --- | --- |
| void | *Return* |  |

### `virtual bool JAREP::Window::IWindowManager::ShouldClose()=0`


| **Type** | **Name** | **Description** |
| --- | --- | --- |
| bool | *Return* | True if the window is about to be destroyed, False if not. |

### `virtual void JAREP::Window::IWindowManager::DestroyWindow()=0`


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
