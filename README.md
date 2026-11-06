# ShadcnWidgetTools Template

基于 [ShadcnWidgetTools](https://github.com/RainbowCandyX/ShadcnWidgetTools) 的 Qt Widgets 应用模板：侧栏导航、明暗主题、设置对话框、中英文切换、安装打包与三平台 CI 都已配好，新建工程后直接添加页面即可。

## 使用模板

1. 在 GitHub 上点击 "Use this template" 创建新仓库，或直接复制本目录。
2. 修改 `CMakeLists.txt` 顶部的变量：

| 变量 | 说明 |
|---|---|
| `APP_NAME` | 应用名称，同时是可执行文件名、CMake 目标名与 macOS bundle 名 |
| `APP_VERSION` | 应用版本，界面中显示，推送 `v<APP_VERSION>` tag 时发布 Release |
| `APP_BUNDLE_ID` | macOS bundle 标识，例如 `com.yourcompany.yourapp` |
| `SHADCN_VERSION` | ShadcnWidgetTools 的版本 tag，升级组件库时修改 |

## 环境要求

| 项目 | 要求 |
|---|---|
| Qt | 6.7 及以上，需要 Qt Linguist Tools；图表 GPU 绘制另需 Qt Shader Tools |
| 编译器 | MSVC、AppleClang、GCC |
| CMake | 3.21 及以上 |
| 网络 | 首次配置时从 GitHub 下载 ShadcnWidgetTools 源码 |

## 构建与运行

通过 `-DQT_SDK_DIR`、环境变量 `QT_SDK_DIR` 或 `CMAKE_PREFIX_PATH` 指定 Qt 安装目录（例如 `~/Qt/6.12.0/macos`）。预设：Windows 为 `msvc-debug` / `msvc-release`（在 Visual Studio x64 开发者命令行中执行），macOS 为 `macos-*`，Linux 为 `linux-gcc-*`。

```bash
cmake --preset macos-release
```

```bash
cmake --build --preset macos-release
```

程序与 ShadcnWidgetTools 动态库输出到 `build/<preset>/bin`。编译后不复制 Qt 运行库，Windows 上需要把 Qt 的 `bin` 目录加入 `PATH` 再运行。

本地修改组件库时，用本地源码代替下载：

```bash
cmake --preset macos-release -DFETCHCONTENT_SOURCE_DIR_SHADCNWIDGETTOOLS=/path/to/ShadcnWidgetTools
```

## 目录结构

```
src/
  main.cpp              程序入口：字体、语言、tooltip 样式
  MainWindow.*          主窗口：侧栏、顶栏、页面栈与主题切换
  SettingsDialog.*      设置：语言、主题、基色与圆角
  AppI18n.h             界面文字随语言刷新的绑定函数
  pages/
    BasePage.*          页面基类：标题、说明与卡片
    HomePage.*          首页
    AboutPage.*         关于页
i18n/
  app_en.ts             英文译文
```

## 添加页面

1. 在 `src/pages` 中新建页面类，继承 `BasePage`，标题与说明以 lambda 传入并在其中调用 `tr()`。
2. 在 `MainWindow` 构造函数中调用 `addPage(mainGroup, ShadIconType::..., new MyPage(_pageStack))`。
3. 把新文件加入 `CMakeLists.txt` 的 `APP_SOURCES`。

组件用法见 ShadcnWidgetTools 的 README 与示例程序。

## 翻译

界面原文为简体中文，`i18n/app_en.ts` 保存英文译文。修改界面文字后构建 `<APP_NAME>_lupdate` 目标更新 ts，再用 Qt Linguist 填写译文：

```bash
cmake --build --preset macos-release --target MyShadcnApp_lupdate
```

新增语言时在 `CMakeLists.txt` 的 `TS_FILES` 中加入 `i18n/app_<语言>.ts`，并在 `SettingsDialog.cpp` 的语言列表中加入对应的 `QLocale`。组件库内置文字自带英文、繁体中文、日语与韩语翻译。

## 打包与发布

`cmake --install build/<preset>` 会把程序、ShadcnWidgetTools 动态库与 Qt 运行库安装到 `install/<preset>`：Windows 全部在 `bin`，Linux 为 `bin`、`lib`、`plugins`，macOS 为 `<APP_NAME>.app`。

`.github/workflows/build.yml` 在 Windows、Linux、macOS 上编译并打包，产物保留 14 天。推送与 `APP_VERSION` 一致的 tag（例如 `v0.1.0`）时，各平台压缩包发布到 GitHub Release；tag 与 `APP_VERSION` 不一致时构建失败。

## 许可证

MIT，可替换为你自己的许可证。ShadcnWidgetTools 使用 MIT 许可证。
