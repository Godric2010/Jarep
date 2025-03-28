# API Documentation

---
## class JAREP::Rendering::IRenderer

**Description**:<br>

---

### Public Functions

### ` ~IRenderer()`


### `bool Initialize(RenderSettings render_settings)`

*Returns: `bool`*<br>*&nbsp;&nbsp;&nbsp;&nbsp;*


### `void Resize(uint32_t width, uint32_t height)`


### `void Shutdown()`


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

