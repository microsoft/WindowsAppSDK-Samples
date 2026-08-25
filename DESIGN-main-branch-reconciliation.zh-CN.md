# Main 分支整合设计

## 问题

当前有四个分支持续独立开发：

- `main`
- `release/experimental`
- `release/2.0-experimental`
- `release/2.0-stable`

`main` 是默认分支，但没有包含所有近期 sample。
`release/experimental` 实际承担了整合分支的作用，与此同时仍有部分变更
继续提交到 `main`。两个 2.0 分支最初用于发布验证，但其中也包含
`release/experimental` 没有的变更。

如果直接按照 commit 历史合并，会同时引入过时版本变更、
experimental-only API、重复 cherry-pick，以及已经被后续实现替代的修改。
因此，本次整合必须以 sample 和最终代码行为为单位，而不能只比较 commit SHA。

## 目标

- 以 `release/experimental` 作为内容整合基线。
- 保留 `main`、`release/experimental` 和 `release/2.0-stable`
  中所有适用的 stable sample。
- 保留仍然适用的独有 bug 修复和构建修复。
- 最终 `main` 使用 stable package 和 stable API。
- 明确识别 experimental-only sample，避免静默加入或删除。
- 最终切换前，独立验证每个迁移的 sample。
- 让 `main` 成为今后 sample 变更的唯一目标分支。

## 非目标

- 保留每个分支特有的 package 版本或发布临时方案。
- Cherry-pick 每一个 SHA 不同的 commit。
- 在 API 稳定性未经确认时，把 experimental-only sample 放入 `main`。
- 改写或删除现有分支历史。
- 在整合期间修改无关代码或批量格式化 sample。

## 固定基线

分析和 integration branch 基于以下远端 commit：

| 分支 | Commit | 日期 |
|---|---|---|
| `main` | `18431c6d` | 2026-08-11 |
| `release/experimental` | `26eddcdf` | 2026-07-01 |
| `release/2.0-experimental` | `63efdc9a` | 2026-07-21 |
| `release/2.0-stable` | `5327f5c4` | 2026-07-02 |

完整 commit ID：

```text
main=18431c6d0111d6a4bd8d46b9c8a82add47ccb013
release/experimental=26eddcdf3240e6f0c89643ca3b7e303bbf3b6a78
release/2.0-experimental=63efdc9a60858dcb9d527569c371830fe58742cf
release/2.0-stable=5327f5c4e47e8661795fd5c25c120868405bdf22
```

本地 integration branch：

```text
integration/main-reconciliation
```

该分支基于 `release/experimental`。

如果任一来源分支在整合期间继续前进，新增 commit 必须单独记录和分类，
不能被隐式带入。

## 分支角色

| 分支 | 整合角色 |
|---|---|
| `release/experimental` | 内容和目录结构基线 |
| `main` | 缺失 stable sample 和较新通用修复的来源 |
| `release/2.0-stable` | Stable API 参考，尤其是 AI sample |
| `release/2.0-experimental` | Experimental-only 内容参考 |

不使用 `release/2.0-stable` 作为仓库基线，是因为它有意省略了部分
sample，并且包含较旧的公共构建和 package 设置。它的独有 patch
主要集中在 `WindowsAIFoundry`，用于移除 experimental API。

## 影响范围

- 公共构建和 package 配置
- Sample 项目文件和 solution 文件
- `WindowsAIFoundry`
- `WindowsML`
- 仅存在于 `main` 的 sample
- 仅存在于 experimental 分支的 sample
- CI workflow 和 sample 索引
- 根目录和 sample 文档

项目边界由 `.csproj`、`.vcxproj`、`.wapproj`、`.sln`、`.slnx`
和 `CMakeLists.txt` 确定。只看 `Samples` 一级目录不够精确。

## 纳入标准

项目进入最终 `main` 时原则上必须：

- 使用正式发布的 package，除非明确记录例外。
- 不依赖 experimental-only API。
- 没有被 stable 发布分支有意删除。
- 能通过仓库现有构建脚本构建。
- 源码、文档和 package 版本描述相同的行为。

每项差异使用以下处置之一：

| 分类 | 处理方式 |
|---|---|
| 两边共享且等价 | 保留基线版本 |
| 通用 bug 或构建修复 | 移植实际效果 |
| 基线缺失的 stable sample | 导入并按基线规范调整 |
| Stable API 版本 | 作为该 sample 的 stable 实现 |
| 特定版本线变更 | 不移植 |
| 已被后续实现替代 | 记录后忽略 |
| Experimental-only 内容 | 排除或隔离，等待决定 |
| 实现冲突 | 手工整合并测试 |

## 实施方案

### 1. 创建项目矩阵

项目级清单包括：

- 在四个分支上的存在状态
- 当前文件树差异
- Package 和 SDK 版本
- Stable 或 experimental API 使用情况
- 独有变更来源
- 建议处理方式
- 构建和 smoke test 状态

使用 rename detection 和 patch equivalence，避免把目录移动或
cherry-pick 误判为无关变更。

矩阵保存在：

```text
MAIN-BRANCH-RECONCILIATION-PROJECT-MATRIX.csv
```

初始分类只描述文件树关系。`Disposition` 和 `Validation` 列会在对应的
审查和构建步骤获批后更新。

#### 变更来源

每项导入或移植的变更都必须记录：

- 来源分支
- 来源 commit
- 来源 PR
- 原始来源路径

项目矩阵使用 `SourceBranch`、`SourceCommit`、`SourcePR` 和
`SourcePath` 保存这些值。来源尚未确认时保持空白，确认后再填写。
如果变更是按路径导入而不是 cherry-pick，后续 commit message 也必须
包含已确认的来源映射。

### 2. 整合公共基础设施

迁移单个 sample 前，先处理：

- `build.ps1` 和 `build.cmd`
- `Directory.Build.props`
- `Directory.Packages.props`
- `Build.Common.Cpp.props`
- `nuget.config`
- CI workflow

从 `main` 移植适用的 C++20、Visual Studio 2026 和编译器兼容修复。
保留 `release/experimental` 的 telemetry suppression 和适用的
bootstrap 设置。除非 sample 明确需要，不复制 `release/2.0-stable`
中的 package 降级。

#### 整合结果

公共基础设施审查保留了基线的构建脚本、中央 package 管理、
telemetry suppression 和 CI 配置。`main` 中 Visual Studio 2026
到 `v145` 的映射已经等价存在于基线中。

已经移植 `main` PR #657 中其余适用变更：

- C++ 项目默认使用 C++20。
- 更新 7 个显式选择 C++17 的 AppLifecycle 项目。
- 修复 UXFrameworksOnIslands 的 dependent base 和 member template
  查找问题。
- 让 SampleWidgetProviderApp 导入公共 C++ toolset 配置。

Widgets x64 Release 构建通过，没有 warning。AppLifecycle 和 Islands
在进入编译前被本机缺少 Visual Studio 2022 `v143` toolset 阻塞。
该环境阻塞已经记录在项目矩阵中，最终验证前必须解决。

### 3. 导入低风险 stable sample

审查 `main` 独有的 stable 项目，包括：

- `SecureUI`
- Stable Islands 项目
- AppLifecycle packaging 项目
- ResourceManagement packaging 项目
- Windowing packaging 项目

每个 sample 或紧密关联的 sample 组，应通过目标为 integration branch
的独立 PR 处理。

#### 候选审查结果

项目级审查确认了一组低风险导入候选：

- `Islands/cs-winforms-unpackaged` 必须和
  `Islands/SampleWinUIClassLibrary` 一起导入。两者通过 project
  reference 组成同一个 sample，并使用 stable Windows App SDK 1.8。

其他 `main`-only 目录不是独立 sample：

- AppLifecycle、ResourceManagement 和 Windowing package 目录是
  基线现有应用的 packaging-project 拆分。暂缓到对应 sample 的
  packaging 审查，不能作为重复 sample 直接导入。
- `Islands/SimpleIslandApp/cpp-win32-unpackaged` 主要是基线
  `Islands/cpp-win32-unpackaged` 的目录迁移和扩展。应与现有路径
  整合，不能添加第二份副本。
- WinML EP Catalog 留到专门的 WindowsML 审查阶段。
- SecureUI 是从 Windowing sample 派生的 isolated-desktop 测试项目，
  需要单独审查，不能作为低风险独立 sample 直接导入。

#### 当前导入状态

- Islands WinForms sample 和 WinUI class library 已经导入。
  x64 Release 构建通过，没有 warning。

已获批的低风险 sample 导入已经完成。

#### SimpleIslandApp 整合结果

保留现有 `Islands/cpp-win32-unpackaged` 路径。已经移植
`main@72b7d1b9`、PR #432 的功能变化：

- 用离线 FlipView 替换依赖网络的 WebView2 内容。
- 添加 FlipView 使用的 5 张图片。
- 在现有项目文件和 filter 文件中注册图片资源。

有意不采用 `main@562c3091`、PR #439 的目录移动。保留基线中央
package 管理和更新的项目配置，没有复制 `main` 中较旧的
`packages.config` 和 package import。

定向 x64 Release 构建被重复的 `WindowsAppRuntimeAutoInitializer.cpp`
项目阻塞。在应用 FlipView 变化前，已提交基线可以复现同一错误，
因此将它记录为既有构建问题，而不是迁移引入的回归。

#### Packaging 项目审查结果

`main` 中 AppLifecycle Activation、Instancing 和 StateNotifications
C++ WinUI sample 的独立 WAP 项目在共同祖先中已经存在。
`release/experimental@0d035e00`、PR #579 主动用 single-project MSIX
替代了它们。应用行为和 manifest 语义没有变化；基线还包含后续 PR
#513 的未使用参数 warning 修复。因此保留基线项目，省略已被取代的
WAP 目录。

ResourceManagement C++ WinUI WAP 项目具有相同历史，也由同一个 PR
替换。它的业务源码完全相同，图片资源逐字节相同，manifest 仅有对应的
`Images` 到 `Assets` 路径变化。保留基线 single-project MSIX 和中央
package 管理。C# WinUI 和 WPF WAP 项目在两个分支中都存在，不属于
此次差异。

Windowing C++ WinUI WAP 项目同样存在于共同祖先中，并由 PR #579
替换。它的业务源码完全相同，7 张图片逐字节相同，manifest 仅有对应的
资源路径差异。保留基线 single-project MSIX，省略旧 WAP 目录。

AppLifecycle、ResourceManagement 和 Windowing 的全部 packaging split
均不需要迁移。

### 4. Stable 化 WindowsAIFoundry

使用 `release/2.0-stable` 作为该 sample 的 Stable API 参考：

- 移除 experimental-only 场景。
- 保留 `release/experimental` 中适用的后续修复。
- 统一为获批的 stable package 版本。
- 不用较旧 release 设置覆盖无关的公共配置。

#### C# WinUI Stable API 结果

C# WinUI gallery 已与 `release/2.0-stable@5327f5c4` 对齐：

- 删除 PR #592 中的 experimental VideoScaler 场景。
- 删除 PR #594 中的 experimental ImageForegroundExtractor 场景和
  `TextRecognizerOptions` 示例。
- 删除重复的 local central package 文件后，继承仓库的 stable Windows
  App SDK 2.4.0 package。
- 保留 6 个 stable 场景，包括 stable LoRA/adapter sample。
- 保留基线编码和无关的后续修复，不覆盖整个 WindowsAIFoundry 文件树。

定向 x64 Release 构建在 restore 阶段被阻塞，因为配置的 feed 不包含
`Microsoft.NETCore.App.Crossgen2.win-x64` 8.0.30。未修改的已提交基线
复现了相同的 `NU1102`，因此这是既有 restore 环境问题，不是 stable API
迁移回归。

#### C#、MAUI 和 WPF Runtime 统一结果

非 CMake 项目现在继承仓库获批的 Windows App SDK `2.4.0` package。
其已恢复的 `Microsoft.WindowsAppSDK.Runtime` metadata 将 framework
identity 定义为 `Microsoft.WindowsAppRuntime.2`，dot-quad 版本为
`2.4.0.0`。

已经统一以下过时的 experimental 引用：

- 将 WinForms 和 WPF sparse manifest 更新到 2.4 stable framework
  identity 和最低版本。
- 将 MAUI、WinForms 和 WPF prerequisite 更新为 stable 1.8 runtime
  和 package。
- 保留 MAUI readiness fallback，但让注释不再绑定过时 experimental
  release。
- 从 `user/qiutongshen/changeExp@acbc15e2` 移植 fully MSIX-packaged
  WPF sample 的修改，删除不必要的 experimental Bootstrap 调用。

packaged WPF、sparse WPF 和 sparse WinForms 的 x64 Release 构建通过，
没有 warning。

root 升级到 Windows App SDK 2.4.0 后，sparse WPF 和 WinForms 再次构建
通过。WinForms 构建还需要将 `SplashScreen.scale-200 (1).png` 重命名为
合法的 PRI qualifier 文件名。

#### CMake Sparse Console Stable Retarget 结果

CMake sparse console 已使用
`user/yeelam/cpp-console-sparse-add-imagescaler@f75df95c` 中的基础设施
部分迁移到 stable `Microsoft.WindowsAppSDK` 2.1.3。没有导入该 feature
branch 的 ImageScaler 场景；sample 仍只关注 `LanguageModel`。

迁移将 sparse dependency 更新为 `Microsoft.WindowsAppRuntime.2`
2.1.3.0，支持 stable NuGet 的 library 和 runtime layout，并修正 README
中对不存在的 `install.ps1` 的过时引用。验证还发现 preset 没有设置
`VCPKG_TARGET_TRIPLET`，因此每个 preset 现在会显式选择对应的 x64 或
ARM64 triplet。

x64 Debug configure、restore、compile、link 和 post-build sparse 注册
通过。本机无法与 nuget.org v3 建立 TLS，因此构建时临时使用仓库公开的
sample dependency feed 获取相同的 2.1.3 package；提交版本继续为外部
使用者保留 nuget.org。`build.ps1` 只在本次验证期间临时适配，之后已完整
恢复，不包含提交修改。

可执行文件成功进入 `LanguageModel::EnsureReadyAsync`，证明 stable
runtime activation 和 package identity 有效，但本机因为没有可用的
language model 返回 `0x80070490`。因此 runtime 场景验证是环境阻塞，
而不是代码阻塞。

### 5. 整合 WindowsML

把 WindowsML 作为手工三方整合：

- 保留 `release/experimental` 目录结构作为起点。
- 按最终行为审查 `main` 的每项独有修复。
- 对路径已经变化的修复进行手工移植，不能盲目 cherry-pick。
- 确认 WinML EP Catalog 可以使用 stable API 后将其导入。
- 验证初始化、CFG、provider matching、日志和 EP 修复。

#### Stable Package 基础和延后决定的 Legacy Sample

采用 `main@f37e15e9`、PR #643 的 stable dependency 集合。WindowsML
central package 文件导入仓库 root，只在本地保留 GenAI WinML 和
AbiWinRT 版本；全部共享 package 版本来自 root。

root graph 升级到 Windows App SDK 2.4.0 后，完整 WindowsML x64 Release
构建通过，没有 warning 或 error。

该 stable package 集合可以用 x64 Release 构建现有 release sample 集合，
没有 warning 或 error。首次构建暴露了既有的并行 SqueezeNet 下载竞争；
共享资源落盘后，后续完整构建通过。

PR #643 还删除了 `CppConsoleDll`、`CppResnetBuildDemo` 和
`ResnetBuildDemoCS`。该删除只存在于 `main`；固定的
`release/experimental`、`release/2.0-experimental` 和
`release/2.0-stable` 都保留了这三个 sample。因为它们可以使用 stable
dependency 集合构建，所以删除不是 stable 迁移的必要条件。

WindowsML 整合期间保留全部三个 legacy sample。将它们的最终处置延后到
integration PR 审查阶段，再根据完整的最终 sample 集合决定。

#### Shared EP 和 Performance Configuration 结果

移植仅存在于 main 的 PR #588、#634、#635、#629、#642 和 #643 行为链。
固定的 experimental 和 stable release 分支都没有对应 cherry-pick。

整合后的行为：

- 为 OpenVINO、QNN、Vitis AI、MIGraphX 和 TensorRT RTX 增加
  `--perf_mode` 支持。
- 生成包含 device 和 performance 信息的 compiled model 路径，避免复用
  不兼容的已编译模型。
- 删除误导性的 `DISABLE` EP policy，并让未知 policy 回退到 `DEFAULT`。
- 避免会导致 access violation 的 Vitis AI option，并使用正确的 TensorRT
  RTX provider 名称。
- Model Catalog JSON 缺失时安全回退，并在 `EnsureReadyAsync` 后报告更新的
  provider state。

已经移植 C++、C#、GenAI、WinForms、WinUI 和 WPF wiring，同时保留 release
分支的 Central Package Management 结构。没有导入 main commit 中过时的
`packages.config` 修改。

完整 WindowsML solution 的 x64 Release 构建通过，没有 warning 或 error。

#### WinUI Application Initialization 结果

移植 `main@b86248e9`、PR #625，在启动 main window 前调用
`App.InitializeComponent()`。固定的 release 分支都没有该修复。缺少调用
不会阻止构建，但 App-level XAML resource 可能无法在运行时加载。

#### WinML EP Catalog 导入决定

从 `main` 导入 `Samples/WindowsML/cpp-cmake/WinMLEpCatalog`。PR #593、
commit `64eee709` 最初在
`Samples/WindowsML/cmake/WinMLEpCatalog` 中新增该 sample；PR #643、
commit `f37e15e9` 将其移动到 `cpp-cmake`，并更新为 stable
`Microsoft.Windows.AI.MachineLearning` 2.1.1 package。

该 sample 与现有 `cpp-abi` 和 inference sample 具有不同的教学价值。
它演示最小 native CMake 应用如何通过 WinML C API 发现和准备 certified
execution provider，再将其 library 动态注册到 ONNX Runtime。它不加载
模型，也不执行 inference。

该 sample 继续作为独立 CMake 项目，不加入 `WindowsML-Samples.sln`。
在仓库根构建脚本明确支持发现 CMake sample 之前，使用其文档中的
`build.ps1` 验证。导入时还修正 README 中的 package 名称，并删除已经过时的
`onnxruntime_providers_shared.dll` troubleshooting 说明。

该 sample 自带的构建脚本已通过 x64 Release CMake configure、restore、
compile 和 link。Smoke test 成功退出，并报告内置 CPU 和 DirectML device；
本机没有可额外注册的 certified provider。

#### Windows ML Diagnostics 导入决定

从 `main@177b65dc`、PR #602 导入 `capture-logs` diagnostics 资源。WPR 和
WPA profile 捕获并整理 stable Windows ML、ONNX Runtime 和 execution
provider ETW event。Microsoft Learn 已发布对应操作流程，并直接链接这些
仓库文件。固定的 release 分支均不包含这些资源。

不原样导入 `Get-WinMLRundown.ps1`。原脚本会静默安装固定版本 Windows
ADK、不检查 WPA Exporter 退出码，并删除调用方输出目录中已经存在的全部
CSV 文件。整合后的脚本改为要求预先安装 Windows Performance Toolkit，
验证输入和 exporter 结果，将中间 CSV 隔离到唯一临时目录，并且只删除该
目录。同时补充仓库要求的 PowerShell help header。

已经在仓库和 WindowsML README 中增加入口。PowerShell 语法和两个 XML
profile 验证通过。定向脚本测试确认可以成功生成 rundown、保留调用方已有的
CSV、清理临时目录，并传播非零 exporter 失败。WPR 可以识别导入的 profile，
但当前 shell 没有管理员权限，真实 trace 捕获被 WPR `0x80070005` 阻塞。
最终整合前需要从管理员 shell 重做 capture-and-convert smoke test。

#### SqueezeNet 下载竞争处置

保留 release 分支来自 PR #556 的 `SqueezeNetModel.targets` 实现。`main`
版本没有修复已经复现的跨项目下载竞争；它删除 command-line build hook，
并改变 output copy 语义。未合入的 copy-fix 分支调整了 build 和 publish
item 注册，但仍允许多个项目同时下载到相同共享文件。

不在 main 分支重建 PR 中加入新的可靠性修复。首次 clean parallel build
可能因多个项目同时填充共享 SqueezeNet cache 而失败；资源存在后，后续构建
可以通过。将它记录为已知既有问题，并通过独立、单一目的的 PR 实现 per-project
download isolation，同时覆盖 clean-cache parallel-build 验证。

#### C++ ABI 安全配置结果

固定 main 和 release 基线中的 `cpp-abi` 源码及 README 完全相同。保留
release 的 Central Package Management 项目配置，并排除 PR #643 的
`packages.config`、生成的 package import 和硬编码 package 路径。

只移植 `main@f5ec17bf`、PR #655。它的 local `Directory.Build.props`
导入 WindowsML parent 设置，并增加其他 native WindowsML sample 使用的
完整 CFG/SDL 配置：compiler 和 linker Control Flow Guard、`/dynamicbase`、
`/Qspectre`、SDL check，以及非 ARM64 build 的 CET compatibility。该修复
接续 PR #648 的 project-level CFG 设置，补齐剩余 BinSkim BA2008 配置缺口，
不改变 sample 行为。
增加 local props 文件后，完整 WindowsML x64 Release 构建通过，没有 warning
或 error。

#### WindowsML 最终差异审计

最终 commit 和文件级审计发现三项尚未进入整合 tree 的适用变更：

- 移植 `main@b82ddd7f`、PR #641，删除 `CppConsoleDll` 中未使用的 function
  pointer typedef，并让预期输出与实际调用的函数一致。Sample 本身仍等待最终
  integration PR 的删除决定。
- 补齐 `main@f37e15e9`、PR #643 的 Python 部分：将两个 Windows App SDK
  Python package 固定到 stable 2.1.3，并删除对 ORT nightly feed 的直接依赖。
  `main` 删除了 SqueezeNet BSD license，但下载脚本仍获取该模型，因此保留
  license。
- 补齐 PR #643 的 C# Model Catalog fallback。Catalog JSON 缺失时改用
  executable-folder model，而不是构造无效 URI。

Release 分支已经通过等价 cherry-pick 包含 PR #577 的 native compiler/linker
switch、PR #578 的 Python environment 重构；其 shared build 配置也等价覆盖
PR #597 的相关 output partitioning。其余源码差异属于 encoding、空白、
generated designer 格式，或有意保留的 release API lifetime 行为。`main`
额外保留的 `Resources/ResNet50` 文件没有任何引用，因此不导入。

Stable Python 2.1.3 dependency graph 在不使用 ORT nightly feed 的情况下解析
成功。完整 WindowsML x64 Release 构建通过，没有 warning 或 error。
WindowsML 整合完成；只保留已经记录的三项后续：最终 integration PR 决定三个
legacy sample、独立 SqueezeNet 可靠性 PR，以及需要管理员权限的 diagnostics
smoke test。

### 6. SecureUI 处置结果

SecureUI 只存在于 `main`，它与 native Windowing AppWindow sample
重构在同一个变更中加入。它复用了 AppWindow、DispatcherQueue、
Composition 和 Mica，但通过 `OpenDesktopW` 和 `SetThreadDesktop`
在隔离 desktop 中创建 UI。

它的来源是 `main` commit `d3922307`，PR #415。

SecureUI 需要单独决定，因为：

- Commit 把它描述为 test project。
- Manifest 使用测试元数据和贡献者个人 Publisher。
- 项目没有 README。
- 它与现有 Windowing sample 大量重叠。
- 从原始 Windows App SDK 1.6 package 模式升级时暴露了额外集成工作。

从 integration 中排除 SecureUI。相比重构后的 native Windowing
sample，它唯一额外的行为是打开预先存在的 `IsolatedTestDesktop`；
仓库没有创建该 desktop，失败时没有错误输出，项目也缺少成为可运行公开
sample 所需的文档和元数据。

已经把 `main@d3922307`、PR #415 的通用 DispatcherQueue 和 AppWindow
重构单独移植到现有 native Windowing sample：

- 使用 Windows App SDK DispatcherQueue 作为应用消息循环。
- 直接创建 AppWindow，并把其生命周期关联到该 queue。
- 在 AppWindow destroying event 中退出消息循环。
- 采用来源变更选择的普通 `Overlapped` presenter。
- 删除自定义 Win32 window class、WNDPROC 和 system DispatcherQueue
  helper。

保留基线中央 package 管理和 Windows App SDK 1.8 配置，没有复制过时的
`packages.config`、显式 1.6 package imports、solution 版本元数据以及
仅 SecureUI 需要的 WIL 依赖。目前 integration branch 不包含 SecureUI
源码。

定向 x64 Release 构建进入编译，并且没有新的源码错误，但 Windows App
SDK Foundation 1.8 重复注入了
`WindowsAppRuntimeAutoInitializer.cpp`。未修改的已提交基线复现了相同
的 `MSB8027` 和 `LNK4042`，因此将其记录为既有 package 集成问题，而
不是 PR #415 引入的回归。

### 7. 决定 experimental 候选

从 stable integration 中排除以下 experimental-only sample：

- `AppContentSearch`
- `WinUI/ConditionalPredicate`

这两个 sample 都不存在于 `release/2.0-stable`，并且都明确依赖 Windows
App SDK 2.0 experimental package。排除决定属于
`DESIGN-stable-2.4-dependency-unification.md` 记录的 stable 2.4
dependency 统一工作。

在 API 确认 stable 之前，不把以下内容加入 stable integration：

- `release/2.0-experimental` 独有的 CMake sample

如果保留，必须明确隔离并标记为 experimental。

### 8. 统一 C++ toolset 选择

只有在最终 sample 集合确定后、最终验证前，才进行全仓库 toolset
统一：

- 继续支持 Visual Studio 2022 和 `v143`。
- 支持 Visual Studio 2026 和 `v145`。
- 在 `Microsoft.Cpp.Default.props` 前统一导入
  `Build.Common.Cpp.props`。
- 移除没有明确理由的硬编码 `v143`。
- 不把硬编码 `v143` 替换成硬编码 `v145`。
- 同时使用 Visual Studio 2022 和 Visual Studio 2026 验证。

迁移 sample 时，暂时保留其原始 toolset 设置。可以在 Visual Studio
2026 中并行安装 `v143`，以解除中间验证阻塞，但这不能替代使用
Visual Studio 2022 的真实验证。

### 9. 更新文档和 CI

Sample 集合获批后：

- 更新根目录 sample 索引和链接。
- 记录支持的 Windows App SDK 版本。
- 明确标记任何获批的 experimental 内容。
- 更新 CI sample 列表。
- 删除过时的分支角色描述。

### 10. 切换到 main

所有实现 PR 都以 integration branch 为目标。最终 PR 以 `main`
为目标，并包含获批的整合结果。

使用保留双方历史的 merge 策略。不能 squash 整个整合历史，也不能
强制更新 `main`。新的 `main` 完成最终验证前，不删除旧 release 分支。

## PR 顺序

1. 设计文档和项目矩阵
2. 公共构建和 package 基础设施
3. 来自 `main` 的低风险 stable sample
4. Stable `WindowsAIFoundry`
5. 整合后的 `WindowsML`
6. SecureUI 处置
7. Experimental 候选决策
8. C++ toolset 统一
9. 文档和 CI
10. Integration 到 `main` 的最终切换

每个 PR 只能包含一个意图，并说明改了什么、为什么修改，以及如何验证。

## 风险

- Package 统一可能改变 sample 行为或支持的 SDK 版本。
- 独有 commit 可能已经以不同路径或实现方式存在。
- Stable release 分支可能混合了有意删除和过时公共配置。
- WindowsML 的大规模路径变化可能使自动 cherry-pick 产生误导。
- 整合期间来源分支可能继续前进。
- 如果不验证最终文件树，最终 merge 可能重新引入不需要的变更。

缓解方式包括固定来源 commit、逐项目决策、聚焦的小 PR、定向构建，
以及最终文件树比较。

## 验证计划

修改构建行为前，必须先阅读并使用仓库现有 `build.ps1` 或
`build.cmd`，不能自行拼装 restore 或 MSBuild 逻辑。

每个迁移的 sample 使用以下方式定向构建：

```powershell
pwsh -File build.ps1 -Sample <SampleName>
```

验证还包括：

- 不产生新的 build warning 或 error
- 不存在未经批准的 prerelease package
- 现有 x64 和 ARM64 配置继续有效
- 适用时对 activation、UI 和 WindowsML 场景进行 smoke test
- 项目矩阵确认没有误删获批 sample
- 最终文件树符合每项获批的项目处置
- 所有定向构建通过后执行全仓库构建

## 完成标准

- 所有获批 stable sample 都存在于 integration branch。
- 每项独有 stable 变更都已纳入或有明确处置记录。
- 每个排除项目都有明确理由。
- 所有受影响 sample 的定向构建通过。
- 全仓库构建通过，且没有新增 warning 或 error。
- 文档和 CI 反映最终 sample 集合。
- 后续贡献统一提交到 `main`。

## 提交记录

该日志记录整合里程碑 commit。只维护日志的文档 commit 不作为里程碑列出。

### `a6655bbe` - 设计和项目矩阵

- 范围：添加中英文设计文档和项目矩阵。
- 验证：Markdown 格式和矩阵完整性检查通过。
- 阶段：设计和清单。

### `e24ee5eb` - 公共 C++ 构建兼容

- 范围：从 `main` PR #657 移植适用的 C++20、编译器兼容和公共
  toolset 修复。
- 来源：`main@18431c6d`，PR #657。
- 验证：Widgets x64 Release 构建通过，没有 warning。AppLifecycle
  和 Islands 在编译前被本机缺少 `v143` 阻塞。
- 阶段：公共基础设施。

### `e7794ab0` - WinForms XAML Islands Sample

- 范围：导入 WinForms XAML Islands sample 及其 WinUI class library，
  并使用中央 package 管理。
- 来源：`main@4d1f233a`、PR #386，以及 `main@aaff62fc`、PR #518。
- 验证：Islands x64 Release 构建通过，没有 warning。
- 阶段：低风险 stable sample。

## 执行状态

- [x] 固定四个来源分支的初始 commit。
- [x] 创建本地 integration branch。
- [x] 建立初始分支和顶层 sample 对比。
- [x] 生成并审查项目级矩阵。
- [x] 整合公共基础设施。
- [x] 审查低风险 stable sample 候选。
- [x] 导入低风险 stable sample。
- [x] 审查 AppLifecycle 和 ResourceManagement packaging split。
- [x] 决定 SecureUI 处置。
- [x] 审查并移植 Windowing PR #415 重构。
- [x] Stable 化 WindowsAIFoundry C# WinUI API surface。
- [x] 统一 WindowsAIFoundry C#、MAUI 和 WPF runtime 引用。
- [x] 将 WindowsAIFoundry CMake sample 迁移到 stable dependency。
- [x] 整合 WindowsML。
- [x] 排除 AppContentSearch 和 ConditionalPredicate。
- [ ] 决定剩余 experimental CMake 候选。
- [ ] 将保留的 sample 统一到 stable Windows App SDK 2.4 graph。
- [ ] 统一 C++ toolset 选择。
- [ ] 更新文档和 CI。
- [ ] 完成最终验证并切换到 `main`。
