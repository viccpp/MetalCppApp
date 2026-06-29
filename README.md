# MetalCppApp

A minimal macOS application using **C++** and **Apple's Metal-cpp** headers.  
The only Objective-C file is `src/platform.mm` (~60 lines) — everything else is pure C++.

## What it does

Renders a colour-interpolated triangle that rotates continuously using a Metal vertex shader.

## Project layout

```
MetalCppApp/
├── CMakeLists.txt
├── README.md
├── metal-cpp/            ← you place the Apple headers here (see below)
└── src/
    ├── main.cpp          ← pure C++: app entry + run loop
    ├── renderer.hpp      ← pure C++: renderer interface
    ├── renderer.cpp      ← pure C++: all Metal-cpp calls
    ├── metal_impl.cpp    ← pure C++: Metal-cpp private implementations
    ├── platform.hpp      ← pure C  : platform abstraction interface
    ├── platform.mm       ← Obj-C   : NSWindow + CAMetalLayer (~60 lines)
    └── shaders.metal     ← MSL     : vertex + fragment shaders
```

## Prerequisites

- macOS 12 or later
- Xcode (for `xcrun`, `metal`, `metallib`, frameworks)
- CMake 3.22+

## Setup

### 1. Download Metal-cpp headers

Go to https://developer.apple.com/metal/cpp/ and download the latest package.  
Unzip it and place the resulting folder as `metal-cpp/` in the project root:

```
MetalCppApp/
└── metal-cpp/
    ├── Metal/
    │   └── Metal.hpp
    ├── Foundation/
    │   └── Foundation.hpp
    └── QuartzCore/
        └── QuartzCore.hpp
```

### 2. Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### 3. Run

```bash
./build/MetalCppApp
```

## Key concepts

| File | Role |
|---|---|
| `metal_impl.cpp` | Defines `NS/CA/MTL_PRIVATE_IMPLEMENTATION` — include in exactly one TU |
| `platform.mm` | The only ObjC: creates `NSWindow` + `CAMetalLayer`, polls `NSApp` events |
| `renderer.cpp` | Full Metal-cpp renderer: pipeline, buffers, render loop |
| `shaders.metal` | MSL shaders compiled to `default.metallib` at build time |
