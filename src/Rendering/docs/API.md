# API Documentation

---
## struct JAREP::Rendering::Core::CameraConfig

**Description**:<br>

---

### Public Fields

`CameraUBO cameraUBO `<br>&nbsp;&nbsp;**Description:**  

`float nearPlane `<br>&nbsp;&nbsp;**Description:**  

`float farPlane `<br>&nbsp;&nbsp;**Description:**  

`bool cullBackFaces `<br>&nbsp;&nbsp;**Description:**  

`bool cullFrontFaces `<br>&nbsp;&nbsp;**Description:**  

---

---
## struct JAREP::Rendering::Core::CameraUBO

**Description**:<br>

---

### Public Fields

`std::array< float, 16 > viewMatrix `<br>&nbsp;&nbsp;**Description:**  

`std::array< float, 16 > projectionMatrix `<br>&nbsp;&nbsp;**Description:**  

`uint32_t isOrthographic `<br>&nbsp;&nbsp;**Description:**  

`uint32_t padding `<br>&nbsp;&nbsp;**Description:**  

---

---
## struct JAREP::Rendering::Core::InstanceData

**Description**:<br>

---

### Public Fields

`std::array< float, 16 > modelMatrix `<br>&nbsp;&nbsp;**Description:**  

---

---
## class JAREP::Rendering::IRenderer

**Description**:<br>

---

### Public Functions

### ` ~IRenderer()`


### `bool Initialize(RenderSettings render_settings, Core::CameraConfig cameraConfig)`

*Returns: `bool`*<br>*&nbsp;&nbsp;&nbsp;&nbsp;*


### `void Resize(uint32_t width, uint32_t height)`


### `void SetRenderResolution(uint32_t resX, uint32_t resY)`


### `void AddRenderObject(JAREP::Core::MeshID meshID, std::shared_ptr< JAREP::Core::Types::Mesh > mesh, Core::InstanceData objectData)`


### `void UpdateRenderObject(Core::RenderObject renderObject)`


### `void DrawFrame()`


### `void Shutdown()`


---

---
## struct JAREP::Rendering::Core::RenderObject

**Description**:<br>

---

### Public Fields

`JAREP::Core::MeshID meshID `<br>&nbsp;&nbsp;**Description:**  

`InstanceData transform `<br>&nbsp;&nbsp;**Description:**  

---

---
## struct JAREP::Rendering::RenderSettings

**Description**:<br>

---

### Public Fields

`SystemType systemType `<br>&nbsp;&nbsp;**Description:**  

`void * handle `<br>&nbsp;&nbsp;**Description:**  

`void * display `<br>&nbsp;&nbsp;**Description:**  

`std::vector< const char * > extensions `<br>&nbsp;&nbsp;**Description:**  

`uint32_t width = 800`<br>&nbsp;&nbsp;**Description:**  

`uint32_t height = 600`<br>&nbsp;&nbsp;**Description:**  

`uint32_t renderWidth = width`<br>&nbsp;&nbsp;**Description:**  

`uint32_t renderHeight = height`<br>&nbsp;&nbsp;**Description:**  

`uint8_t msaa = 1`<br>&nbsp;&nbsp;**Description:**  

---

---
## struct JAREP::Rendering::Core::SamplerConfig

**Description**:<br>

---

### Public Fields

`VkFilter magFilter = VK_FILTER_LINEAR`<br>&nbsp;&nbsp;**Description:**  

`VkFilter minFilter = VK_FILTER_LINEAR`<br>&nbsp;&nbsp;**Description:**  

`VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT`<br>&nbsp;&nbsp;**Description:**  

`VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT`<br>&nbsp;&nbsp;**Description:**  

`VkSamplerAddressMode addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT`<br>&nbsp;&nbsp;**Description:**  

`VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR`<br>&nbsp;&nbsp;**Description:**  

`float mipLodBias = 0.0f`<br>&nbsp;&nbsp;**Description:**  

`float minLod = 0.0f`<br>&nbsp;&nbsp;**Description:**  

`float maxLod = VK_LOD_CLAMP_NONE`<br>&nbsp;&nbsp;**Description:**  

`bool anisotropyEnable = false`<br>&nbsp;&nbsp;**Description:**  

`float maxAnisotropy = 1.0f`<br>&nbsp;&nbsp;**Description:**  

`bool unnormalizedCoordinates = false`<br>&nbsp;&nbsp;**Description:**  

`bool compareEnable = false`<br>&nbsp;&nbsp;**Description:**  

`VkCompareOp compareOp = VK_COMPARE_OP_LESS`<br>&nbsp;&nbsp;**Description:**  

`VkBorderColor borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK`<br>&nbsp;&nbsp;**Description:**  

---

