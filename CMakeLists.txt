cmake_minimum_required(VERSION 3.21)

# 应用名称、版本与 macOS bundle 标识；新建工程后修改这里
set(APP_NAME "MyShadcnApp")
set(APP_VERSION "1.0.0")
set(APP_BUNDLE_ID "com.example.myshadcnapp")
# ShadcnWidgetTools 版本，对应其仓库的 vX.Y.Z tag
set(SHADCN_VERSION "v1.0.0")

project(${APP_NAME} VERSION ${APP_VERSION} LANGUAGES CXX)

# 构建类型：多配置生成器（Visual Studio）提供 Debug / Release；单配置生成器（Ninja / Makefiles）未指定时默认 Release
get_property(_app_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (_app_is_multi_config)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "可用的构建配置" FORCE)
elseif (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "构建类型" FORCE)
endif ()
unset(_app_is_multi_config)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 路径由 -DQT_SDK_DIR、环境变量 QT_SDK_DIR 或 CMAKE_PREFIX_PATH 指定
if (NOT DEFINED QT_SDK_DIR AND DEFINED ENV{QT_SDK_DIR})
    file(TO_CMAKE_PATH "$ENV{QT_SDK_DIR}" _app_env_qt_dir)
    set(QT_SDK_DIR "${_app_env_qt_dir}" CACHE PATH "Qt SDK 根目录")
    unset(_app_env_qt_dir)
endif ()
if (QT_SDK_DIR)
    list(PREPEND CMAKE_PREFIX_PATH "${QT_SDK_DIR}")
    list(REMOVE_DUPLICATES CMAKE_PREFIX_PATH)
endif ()

# Qt 6.7 起 qt_add_translations 支持 SOURCE_TARGETS，只从本应用提取待翻译文字，不收集组件库的源码
find_package(Qt6 6.7 REQUIRED COMPONENTS Widgets LinguistTools)
qt_standard_project_setup(REQUIRES 6.7)
get_filename_component(APP_QT_DIR "${Qt6_DIR}/../../.." ABSOLUTE)
message(STATUS "当前 Qt 路径: ${APP_QT_DIR}")

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE PATH "安装路径" FORCE)
endif ()

# 程序与 ShadcnWidgetTools 动态库输出到同一目录，Windows 上可直接运行；需在引入组件库之前设置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# 引入组件库源码一起编译；本地开发组件库时可用 -DFETCHCONTENT_SOURCE_DIR_SHADCNWIDGETTOOLS=<本地仓库路径> 改用本地源码
include(FetchContent)
FetchContent_Declare(ShadcnWidgetTools
    GIT_REPOSITORY https://github.com/RainbowCandyX/ShadcnWidgetTools.git
    GIT_TAG ${SHADCN_VERSION}
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(ShadcnWidgetTools)

# 新增源文件后加入此列表
set(APP_SOURCES
    src/main.cpp
    src/AppI18n.h
    src/MainWindow.h
    src/MainWindow.cpp
    src/SettingsDialog.h
    src/SettingsDialog.cpp
    src/pages/BasePage.h
    src/pages/BasePage.cpp
    src/pages/HomePage.h
    src/pages/HomePage.cpp
    src/pages/AboutPage.h
    src/pages/AboutPage.cpp
)

# macOS 上生成 .app bundle，install 时 Qt 部署脚本只支持 bundle
qt_add_executable(${APP_NAME} WIN32 MACOSX_BUNDLE ${APP_SOURCES})

target_include_directories(${APP_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pages
)

target_link_libraries(${APP_NAME} PRIVATE
    Qt6::Widgets
    ShadcnWidgetTools::ShadcnWidgetTools
)

# 应用名称与版本供 QCoreApplication 使用
target_compile_definitions(${APP_NAME} PRIVATE APP_NAME="${APP_NAME}" APP_VERSION="${APP_VERSION}")

# 源码按 UTF-8 读取，字符串字面量按 UTF-8 编码；MSVC 与 clang-cl 默认使用本地代码页，GCC 默认跟随 locale
if (MSVC)
    target_compile_options(${APP_NAME} PRIVATE /utf-8)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${APP_NAME} PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif ()

set_target_properties(${APP_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "${APP_BUNDLE_ID}"
    MACOSX_BUNDLE_BUNDLE_NAME "${APP_NAME}"
    MACOSX_BUNDLE_BUNDLE_VERSION "${APP_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${APP_VERSION}"
    # Visual Studio 调试时从 Qt 的 bin 目录加载 Qt 运行库
    VS_DEBUGGER_ENVIRONMENT "PATH=${APP_QT_DIR}/bin;%PATH%"
)

# 原文为简体中文，ts 只保存译文；.qm 嵌入资源 :/i18n，main.cpp 通过 ShadI18n 注册
# 修改界面文字后构建 <APP_NAME>_lupdate 目标更新 ts，再用 Qt Linguist 填写译文
qt_add_translations(${APP_NAME}
    SOURCE_TARGETS ${APP_NAME}
    TS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/i18n/app_en.ts"
    RESOURCE_PREFIX "/i18n"
)

# Linux 的 install RPATH 由 qt_standard_project_setup 设置为 $ORIGIN/../lib；
# macOS 的 bundle 位于 <prefix>/<APP_NAME>.app，可执行文件向上三级即 <prefix>，macdeployqt 依赖该 RPATH 找到 ShadcnWidgetTools 并复制进 bundle
if (APPLE)
    set_target_properties(${APP_NAME} PROPERTIES
        INSTALL_RPATH "@executable_path/../../../${CMAKE_INSTALL_LIBDIR}"
    )
endif ()

# 编译后不复制 Qt 运行库；install 时由 Qt 部署脚本复制运行库，用于打包发布
install(TARGETS ${APP_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION .
)
qt_generate_deploy_app_script(
    TARGET ${APP_NAME}
    OUTPUT_SCRIPT APP_DEPLOY_SCRIPT
    NO_UNSUPPORTED_PLATFORM_ERROR
)
# Windows 上 Qt 插件与 exe 一起放在 bin，发布时只需 bin 目录；部署脚本在同一作用域读取该变量
if (WIN32)
    install(CODE "set(QT_DEPLOY_PLUGINS_DIR \"${CMAKE_INSTALL_BINDIR}\")")
endif ()
install(SCRIPT ${APP_DEPLOY_SCRIPT})
