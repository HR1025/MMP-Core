# README

## 简介

MMP (Multi Media Plug-in) 多媒体插件核心基础库.

## 编译

`MMP` 目前仅支持运行在 `linux` 和 `windows` 平台, 短期内功能开发集中在 `linux`.

### debian desktop environment

在 `debian` 环境下, 可以运行以下命令进行构建:

```shell
# debian 环境下
# [option] USE_OPENGL
sudo apt install libgl1-mesa-dev* nasm
# [option] USE_SDL
sudo apt install libsdl2-dev
# [option] USE_X11
sudo apt install libx11-dev
# [option] USE_GBM
sudo apt install libgbm-dev
# [option] USE_GBM 、USE_VAAPI
sudo apt install libdrm-dev
# [option] USE_VAAPI
sudo apt install libva-dev
rm -rf build
mkdir build
git submodule update --init --recursive
cd build
cmake .. -DUSE_OPENGL=ON -DUSE_EGL=ON -DUSE_GBM=ON -DUSE_SDL=ON -DUSE_X11=ON -DUSE_VAAPI=ON
make -j4
```

### x86 windows MSVC 2022

在 Windows 上 `MMP`, 通过 `vcpkg` 来管理依赖, 为了方便将 `vcpkg` 作为一个子仓库管理.

> See also : [vcpkg](https://github.com/microsoft/vcpkg/blob/master/README_zh_CN.md)

在第一次编译项目前, 需要生成 `vcpkg.exe` 以及使用其下载依赖项:

```shell
# Power Shell
# 生成 vcpkg.exe
git submodule update --init --recursive
git clone https://github.com/microsoft/vcpkg.git Extension/vcpkg
.\Extension\vcpkg\bootstrap-vcpkg.bat
# 安装 SDL
.\Extension\vcpkg\vcpkg.exe install sdl2:x86-windows
```

```shell
# Git Bash
mkdir build
cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -S. -B./build -G "Visual Studio 17 2022" -T host=x86 -A win32
cmake --build build --config Release -j 16
```

### raw linux environment

在许多场景下,嵌入式 linux 操作系统通常为定制系统,而非 `debian`, 这时可以通过以下命令进行**初步**构建:

```shell
sudo apt install nasm
rm -rf build
mkdir build
git submodule update --init --recursive
cd build
cmake ..
make -j4
```

在**初步**编译成功后,可以通过手动配置依赖开启其他功能,如下:

> `MMP` 使用 `find_package` 来进行依赖管理,这个功能通常在桌面系统下才支持.

#### Enable EGL

```cmake
set(USE_EGL ON)
set(OPENGL_FOUND ON)
set(OPENGL_INCLUDE_DIR `EGL header directory`)
set(OpenGL_LIBRARY_DIRS `EGL lib directory`)
add_library(OpenGL::OpenGL as `EGL lib`)
```

#### Enabel OpenGL

```cmake
set(USE_OPENGL ON)
set(OPENGL_FOUND ON)
set(OPENGL_INCLUDE_DIR `OpenGL header directory`)
set(OpenGL_LIBRARY_DIRS `OpenGL lib directory`)
add_library(OpenGL::OpenGL as `OpenGL lib`)
```

#### Enable SDL

> 对于一个原生linux系统,应该没有SDL吧.

### FAQ

#### 交叉编译如何寻找这些库

应该在你的系统上就已经存在了! `MMP` 几乎所有代码都是从源码开始进行编译,只有两种例外情况:

- 源码不可获取
- 依赖库非可独立构建,或构建难度过大

#### 支持 Window 、MacOS 、IOS 或 Android 吗

长期内有打算,短期没有.

MMP 短期内的方向在于完善功能,基于 `linux` 平台,以嵌入式 `arm linux` 作为实际应用场景,以 `x86 debian` 作为调试环境.

> 换句话说, MMP 面向是客户端,并不适合高并发的场景.

但是 `MMP` 本身是具有跨平台的可能性的!

## 调试

`MMP-Core` 实际上是作为 `MMP`` 的子仓库进行使用,调试相关部分位于 [MMP](https://github.com/HR1025/MMP).

## 工程结构

- Common : 基础组件库
- Extension : 第三方依赖
  - poco : 公共基础组件
  - GLEW : OpenGL 拓展辅助
  - lodepng : 轻量级 png 编解码实现
  - SPIRV-Cross : 跨平台 SL(Shader Language) 转换器
  - DirectX : Direct X (Microsoft)
  - Vulkan-Headers : Vulkan 头文件
  - VulkanMemoryAllocator : vulkan 内存分配管理
- GPU : 通用 GPU 基础库
  - Windows: EGL or SDL or DXGI
  - GL : OpenGL (ES3) or D3D11
- Codec : 编解码器
  - PNG : PNG 编解码实现
  - H264 : H264 语法解析
  - VAAPI : LIBVA 硬编解码
- RPC : 远端调用
- Cmake : 通用 Cmake 实现 或 `Submoudle Cmake Wrapper`
- LICENSES : 使用到依赖库的相关 LICENSE