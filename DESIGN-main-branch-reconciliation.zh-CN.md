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
- 使用 PR #638 选择的 stable Windows App SDK 2.1.3 package。
- 保留 6 个 stable 场景，包括 stable LoRA/adapter sample。
- 保留基线编码和无关的后续修复，不覆盖整个 WindowsAIFoundry 文件树。

定向 x64 Release 构建在 restore 阶段被阻塞，因为配置的 feed 不包含
`Microsoft.NETCore.App.Crossgen2.win-x64` 8.0.30。未修改的已提交基线
复现了相同的 `NU1102`，因此这是既有 restore 环境问题，不是 stable API
迁移回归。

其余 WindowsAIFoundry 项目仍有 experimental runtime、bootstrap 和
README 引用，而且固定的 stable 分支也保留了这些内容。需要在确认并
验证获批的 stable runtime identity 和版本后，单独统一这些跨项目引用。

### 5. 整合 WindowsML

把 WindowsML 作为手工三方整合：

- 保留 `release/experimental` 目录结构作为起点。
- 按最终行为审查 `main` 的每项独有修复。
- 对路径已经变化的修复进行手工移植，不能盲目 cherry-pick。
- 判断 WinML EP Catalog 是否可以使用 stable API。
- 验证初始化、CFG、provider matching、日志和 EP 修复。

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

在 API 确认 stable 之前，不把以下内容加入 stable integration：

- `AppContentSearch`
- `WinUI/ConditionalPredicate`
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
- [ ] 统一其余 WindowsAIFoundry runtime 和文档引用。
- [ ] 整合 WindowsML。
- [ ] 决定 experimental 候选。
- [ ] 统一 C++ toolset 选择。
- [ ] 更新文档和 CI。
- [ ] 完成最终验证并切换到 `main`。
