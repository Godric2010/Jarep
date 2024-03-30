# TODOs

The graphics wrapper is an ever growing feature of JAREP. This list provides all the steps necessary in the future
as well as the steps successfully done so far.

## Step 1: Basic rendering in Vulkan and Metal
-[x] Display a basic triangle in metal
-[x] Display a basic triangle in vulkan
-[x] Check if Physical and logical device creation can be merged into one in JargAPI
-[x] Build shared interface for device and surface
-[x] Build shared interface for pipelines
-[x] Build shared interface for Buffers
-[x] Take care of shader transpilation from GLSL to SPRI-V (vulkan) / MetalShader (metal)

## Step 2: Multiple Render Pipelines
-[ ] Bundle pipeline data into a "RenderStep" object
-[ ] Render each RenderStep in a separate render pass
-[ ] Optimize JarRenderer interface
  - [ ] Make multisampling detection and options available to the outside
  - [ ] Make common rasterization settings available to the outside
  - [ ] Make available depth settings public to the outside
  - [ ] Write high level test for testing the renderer with various settings enabled or disabled
  - [ ] Write high level documantion for the renderer

## Step 3: Mesh and Material Management
- [ ] Add a manager class for loaded textures
- [ ] Add a manager class for loaded meshes
- [ ] Add a manager class for loaded materials
- [ ] Add a manager class for loaded shaders
- [ ] Bind Meshes and Materials to Entities
- [ ] Add a camera entity
- [ ] Manage transform stuff inside the render step and respected camera
- [ ] Make Renderer call the entities and managers to render stuff