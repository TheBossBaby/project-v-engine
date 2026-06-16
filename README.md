# Project V Engine

Project V Engine is an experimental C++ engine that implements core abstractions from **project-v-core** and will later be used by a separate game project. It focuses on a clean architecture, explicit engine conventions, and a modular rendering backend.

## Engine conventions

- Coordinate system: **left‑handed, Z‑up** (similar to Unreal Engine).  
- Front‑face winding: **counter‑clock‑wise (CCW)**.  
- Matrix layout: **column‑major** on the engine side.  
- UV convention: currently follows Assimp’s default; UV flipping can be enabled per import.

These conventions are applied when importing meshes through Assimp to keep engine data consistent.

## External dependencies (submodules)

This repository uses several external libraries as git submodules:

- **Assimp** – Open Asset Import Library  
  - Repository: https://github.com/assimp/assimp  
  - Purpose: Loading 3D model files (e.g., FBX, OBJ) into a unified in‑memory format.  
  - Integrated as a subdirectory under `external/assimp`. [web:476][web:503]

- **GLFW** – Multi‑platform window and input library  
  - Repository: https://github.com/glfw/glfw  
  - Purpose: Window creation, input handling.

- **spdlog** – Fast C++ logging library  
  - Repository: https://github.com/gabime/spdlog  
  - Purpose: Logging engine and application events.

- **project-v-core** – Core interfaces and abstractions  
  - Repository: https://github.com/TheBossBaby/project-v-core  
  - Purpose: Defines engine‑agnostic interfaces for application, window, renderer, and resources.

## Mesh loading status

Mesh loading is currently implemented via **MeshLoaderAssimp**, which converts Assimp data into the engine’s `Mesh` format.

Current limitations:

- **Single mesh per file**:  
  Only the **first mesh** in the imported Assimp scene is processed. Files containing multiple meshes or complex scene hierarchies are not yet supported.

- No material or animation support yet.

These limitations will be relaxed in later iterations as the engine’s resource and rendering systems mature.

## MeshManager

MeshManager is responsible for loading and caching mesh assets. It owns all loaded meshes and returns MeshHandle values that can be used to query meshes later.

### Caching behavior and limitation

- Meshes are cached by file path only. The first successful call to MeshManager::load for a given path uses whatever MeshLoadParams flags are provided (for example bFlipUVs, bCenterMesh, bNormalizeScale). Subsequent calls with the same path will always reuse the existing cached mesh and ignore any different flag combinations.

- This design keeps memory usage simple and predictable, but it also means you cannot load multiple variants of the same source file (for example both centered and non‑centered versions) at the same time through MeshManager in the current version. If you need that behavior, you will need to either use different logical paths for pre‑baked variants or extend MeshManager so that its cache key includes the relevant MeshLoadParams flags.

## Build

This project uses CMake and git submodules.

### 1. Clone the repository

```bash
git clone --recursive https://github.com/TheBossBaby/project-v-engine.git
cd project-v-engine
```

If you already cloned without `--recursive`, run:

```bash
git submodule update --init --recursive
```

### 2. Configure the build

Create a separate `build` directory and generate build files.

**Windows (Visual Studio generator, default):**

```bash
cmake -S . -B build
```

**Linux/macOS (Ninja or Makefiles, pick one you prefer):**

```bash
cmake -S . -B build -G "Ninja"
# or
cmake -S . -B build -G "Unix Makefiles"
```

### 3. Build the engine

**Multi-config generators (Visual Studio, Xcode):**

```bash
cmake --build build --config Debug
# or
cmake --build build --config Release
```

**Single-config generators (Ninja, Makefiles):**

```bash
cmake --build build
```

### 4. Run the application

The default target builds the application under `apps/application`:

- On Windows + Visual Studio: open `build/project_v_engine.sln` and run the `project_v_app` target, or use `cmake --build` as above and run the generated executable from `build/apps/application/...`.
- On Ninja/Makefiles: run the executable produced in the `build/apps/application` directory (e.g. `./apps/application/project_v_app`).

> Note: the engine uses GLFW for windowing, Assimp for mesh loading, spdlog for logging, and EnTT for ECS. All of these are provided as git submodules and are built automatically by CMake; you do not need to install them separately.
