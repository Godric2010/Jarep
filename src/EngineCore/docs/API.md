# API Documentation

---
## class JAREP::Core::IMeshLibrary

**Description**:<br>The mesh library contains all meshes available into the memory at runtime.

---

### Public Functions

### ` ~IMeshLibrary()`


### `MeshID LoadMesh(Types::Mesh mesh)`

**Description**:<br>*Add a new mesh to the library in memory. 

mesh


The mesh to add. 



The ID of the Mesh inside the library (HashCode)*

*Returns: `MeshID`*<br>*&nbsp;&nbsp;&nbsp;&nbsp;The ID of the Mesh inside the library (HashCode)*

*`Types::Mesh mesh`*<br>&nbsp;&nbsp;&nbsp;&nbsp;*The mesh to add.* 


### `void UnloadMesh(MeshID id)`

**Description**:<br>*Remove a mesh from the library and therefore the memory. 

id


The id of the mesh to remove.*

*`MeshID id`*<br>&nbsp;&nbsp;&nbsp;&nbsp;*The id of the mesh to remove.* 


### `Types::Mesh GetMesh(MeshID id)`

**Description**:<br>*Get a mesh from the library. 

id


The hash-id of the mesh. 



The mesh behind this id.*

*Returns: `Types::Mesh`*<br>*&nbsp;&nbsp;&nbsp;&nbsp;The mesh behind this id.*

*`MeshID id`*<br>&nbsp;&nbsp;&nbsp;&nbsp;*The hash-id of the mesh.* 


---

---
## struct JAREP::Core::Types::Mesh

**Description**:<br>

---

### Public Fields

`std::vector< Vertex > vertices `<br>&nbsp;&nbsp;**Description:**  

`std::vector< uint32_t > indices `<br>&nbsp;&nbsp;**Description:**  

---

---
## struct JAREP::Core::Types::Vertex

**Description**:<br>

---

### Public Functions

### `bool operator==(const Vertex &other)`

*Returns: `bool`*<br>*&nbsp;&nbsp;&nbsp;&nbsp;*


### `bool operator!=(const Vertex &other)`

*Returns: `bool`*<br>*&nbsp;&nbsp;&nbsp;&nbsp;*


### Public Fields

`glm::vec3 position `<br>&nbsp;&nbsp;**Description:**  

`glm::vec2 uv `<br>&nbsp;&nbsp;**Description:**  

---

