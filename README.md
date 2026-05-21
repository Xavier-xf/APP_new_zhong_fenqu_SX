## 2026-05-21

### 问题描述：

SX 项目原语言文案主要依赖 `language.c` 中的硬编码二维数组，新增语言时需要同时改代码和资源，维护成本高，并且容易因为表格顺序、空行、语言数量变化导致显示错位。

本次需求是将 `APP_new_zhong_fenqu_SX` 的多语言文案改为优先从 `res/language.xls` 读取，字库使用 `res/sat_leo.ttf`，并确保升级包中包含当前 app、`language.xls`、`sat_leo.ttf` 和 `rom.bin`。

### 问题原因：

1. 原硬编码语言表只适合少量语言，新增语言需要修改大量数组内容。
2. 早期尝试按固定行号或 ID 读取 xls 时，表格插入空行、调整顺序后容易造成代码与表格错位。
3. SX 的 `language.xls` 在目标板上存在 UTF-16 解码兼容问题，直接使用 libxls 返回值会出现 `*failed to decode utf16*` 或空字符串。
4. 语言选择列表需要显示 xls 中的语言名称，但不能影响原有 checkbox 的点击和按下效果。

### 解决方法：

1. 启动时加载 `/app/app/language.xls`，将 xls 内容缓存到内存中。
2. 运行时使用“英文列作为 key”的方式查找文案：代码中保留英文 fallback，查找 xls 英文列中相同的英文 key，再返回当前语言列文本。
3. xls 查找失败、目标语言单元格为空或 xls 未初始化时，自动回退到 `language.c` 中原硬编码内容，保证设备不会因为表格缺项而空白。
4. 对英文 key 先做精确匹配，失败后再做归一化匹配。归一化会忽略大小写、空格和符号，只比较英文字母与数字，用于兼容 `Auto recording`/`Auto Recording` 这类差异。
5. 对同一张代码表内重复英文 fallback，按出现次数匹配 xls 中第 N 个同名英文 key，避免多个重复项都命中第一条。
6. 语言选择列表的语言名称通过查找英文列为 `English` 的那一行获取各语言列内容；如果读取失败则回退到内置语言名。
7. `language.xls` 第一列 ID 当前只作为人工维护和排查用，运行时不依赖 ID，也不依赖绝对行号。
8. `sat_leo.ttf` 作为多语言字体资源，打升级包时必须同步到内核 rootfs/resource 的 app 目录。

### 涉及文件：

- `APP_new_zhong_fenqu_SX/res/language.xls`
- `APP_new_zhong_fenqu_SX/res/sat_leo.ttf`
- `APP_new_zhong_fenqu_SX/layout/lang_xls.c`
- `APP_new_zhong_fenqu_SX/layout/lang_xls.h`
- `APP_new_zhong_fenqu_SX/layout/language.c`
- `APP_new_zhong_fenqu_SX/layout/language.h`
- `APP_new_zhong_fenqu_SX/layout/layout_logo.c`
- `APP_new_zhong_fenqu_SX/layout/layout_setting_language.c`
- `APP_new_zhong_fenqu_SX/share/xls/xlstool.c`
- `meiou_AK37D_fenqu/rootfs/rootfs/app/app/language.xls`
- `meiou_AK37D_fenqu/rootfs/rootfs/app/app/sat_leo.ttf`
- `meiou_AK37D_fenqu/rootfs/resource/app/app/language.xls`
- `meiou_AK37D_fenqu/rootfs/resource/app/app/sat_leo.ttf`
- `meiou_AK37D_fenqu/upgrade/ME_AHD_ANYKA.IMG`

### 具体修改：

#### 1. xls 读取链路

`lang_xls.c` 新增 xls 初始化和缓存逻辑：

```c
#define XLS_TMP_PATH "/app/app/language.xls"
```

设备启动后在 `layout_logo.c` 调用 `init_language_xls_info()` 初始化语言表，再通过 `language_id_set(user_data_get()->etc.language)` 设置当前语言。

xls 列定义在 `lang_xls.h`：

```c
XLS_LANG_COL_PAGE = 0
XLS_LANG_COL_ENGLISH = 1
XLS_LANG_COL_ARABIC = 2
XLS_LANG_COL_HUNGARIAN = 3
XLS_LANG_COL_SLOVAK = 4
XLS_LANG_COL_ROMANIAN = 5
XLS_LANG_COL_SERBIAN = 6
XLS_LANG_COL_GERMAN = 7
XLS_LANG_COL_POLISH = 8
XLS_LANG_COL_PORTUGUESE = 9
XLS_LANG_COL_FRENCH = 10
XLS_LANG_COL_CHINESE = 11
```

#### 2. 语言枚举到 xls 列映射

`language.c` 中通过 `language_to_xls_col()` 将 APP 内部语言枚举映射到 xls 列：

```c
LANGUAGE_ID_ENGLISH -> XLS_LANG_COL_ENGLISH
LANGUAGE_ID_ALABOYU -> XLS_LANG_COL_ARABIC
LANGUAGE_ID_XIONGYALIYU -> XLS_LANG_COL_HUNGARIAN
LANGUAGE_ID_SILUOFAKEYU -> XLS_LANG_COL_SLOVAK
LANGUAGE_ID_LUOMANIYAYU -> XLS_LANG_COL_ROMANIAN
LANGUAGE_ID_SAIBEIYAYU -> XLS_LANG_COL_SERBIAN
LANGUAGE_ID_DEYU -> XLS_LANG_COL_GERMAN
LANGUAGE_ID_BOLANYU -> XLS_LANG_COL_POLISH
LANGUAGE_ID_PUTAOYAYU -> XLS_LANG_COL_PORTUGUESE
LANGUAGE_ID_FAYU -> XLS_LANG_COL_FRENCH
LANGUAGE_ID_CHINESE -> XLS_LANG_COL_CHINESE
```

#### 3. 文案获取方式

原代码调用方式保持不变，例如：

```c
layout_setting_etc_string_get(SETTING_ETC_LANG_ID_LANGUAGE)
```

内部流程变为：

```text
代码英文 fallback -> 查找 language.xls 英文列 -> 取当前语言列 -> 失败则回退硬编码
```

这意味着旧代码里的英文字符串仍然很重要，它是 xls 查找 key，不建议随意改英文 fallback。

#### 4. 空行和空单元格处理

- xls 中没有 ID 的空行不会影响运行时查找。
- 空行会被跳过。
- 当前语言列为空时，会回退到硬编码内容。
- 英文列为空时，该行不会被代码匹配。
- 如果英文 key 重复，代码会按同一代码表内的出现次数匹配对应的重复行。

#### 5. 语言选择列表

语言选择列表显示的语言名称优先来自 `language.xls`：

```text
查找英文列内容为 English 的行 -> 读取每个语言列 -> 显示为语言名称
```

例如该行中各列可以维护为：

```text
English / العربية / Magyar / Maďarský / Română / Srpski / Deutsch / Polski / Portuguese / French / 中文
```

如果该行某列为空或异常，则回退到代码中的默认语言名。

#### 6. 字体和打包

新增或更新语言后，尤其是涉及重音字符、阿语、中文等字符集时，需要同步更新 `res/sat_leo.ttf`。

打升级包前必须确认以下文件进入内核 rootfs/resource 输入目录：

```text
CDV1004QT.BIN
language.xls
sat_leo.ttf
rom.bin
```

升级包必须通过内核目录打包生成：

```bash
cd meiou_AK37D_fenqu/upgrade
./partition_image.sh app_resource
```

最终使用的升级包为：

```text
meiou_AK37D_fenqu/upgrade/ME_AHD_ANYKA.IMG
```

### 后续维护方案：

#### 新增普通界面文案

1. 先确认代码里已经有英文 fallback，例如 `language.c` 某个二维语言表中的英文列。
2. 在 `APP_new_zhong_fenqu_SX/res/language.xls` 新增一行。
3. xls 英文列必须填写与代码英文 fallback 一致的英文 key。
4. 在各语言列填写对应翻译。
5. 第一列 ID 可按维护习惯填写，也可以留空；运行时不使用第一列 ID。
6. 编译并打包，确认升级包内 `language.xls` 与 `res/language.xls` 一致。

#### 修改已有翻译

1. 在 `language.xls` 中找到英文列对应的 key。
2. 只修改目标语言列内容。
3. 不要随意修改英文列；英文列是运行时查找 key。
4. 如果必须修改英文列，需要同步检查代码里的英文 fallback 是否仍能匹配。
5. 重新打包，并确认 `language.xls` 已同步进升级包。

#### 删除文案

1. 如果代码仍在使用该英文 key，不建议删除整行。
2. 如果只是想让某个语言回退到硬编码，可以清空该语言列。
3. 如果确认代码不再使用该文案，可以删除整行。
4. 删除空行不会导致后续文案错位，因为运行时不依赖行号。

#### 新增语言

新增语言列属于代码和表格同时修改，不能只改 xls。

需要修改：

1. `language.h`：新增 `LANGUAGE_ID_*`，并更新 `LANGUAGE_ID_TOTAL`。
2. `lang_xls.h`：新增 `XLS_LANG_COL_*`，并更新 `XLS_LANG_COL_TOTAL`。
3. `language.c`：在 `language_to_xls_col()` 中增加语言枚举到 xls 列的映射。
4. `language.xls`：新增一列，并补齐所有需要显示的翻译。
5. `lang_xls_language_name_get()`：补充语言名称 fallback。
6. `sat_leo.ttf`：确认字体包含新增语言所需字符。
7. 重新编译、同步资源、重新打包升级包。

#### 删除语言

删除语言也需要代码和表格同时修改：

1. 删除或停用对应 `LANGUAGE_ID_*`。
2. 删除或停用对应 `XLS_LANG_COL_*`。
3. 更新 `language_to_xls_col()`。
4. 从 `language.xls` 删除对应语言列。
5. 检查语言选择列表是否仍能正常显示和选择。
6. 重新编译并打包验证。

#### 修改英文 key 的注意事项

英文列是 xls 查找 key。维护时优先保持英文列稳定。

如果出现界面没有翻译、回退为英文或硬编码，优先检查：

1. `language.xls` 英文列是否和代码英文 fallback 一致。
2. 当前语言列是否为空。
3. `language.xls` 是否已经打包进 `/app/app/language.xls`。
4. `sat_leo.ttf` 是否已经同步进升级包。
5. 设备启动日志是否有 `[language_xls] init success`。

#### 推荐验证方法

每次改语言表或字体后，至少验证：

1. 编译生成新的 `CDV1004QT.BIN`。
2. 同步 `CDV1004QT.BIN`、`language.xls`、`sat_leo.ttf`、`rom.bin` 到内核 rootfs/resource 两处 app 目录。
3. 重新生成 `platform/app.sqsh4` 和 `ME_AHD_ANYKA.IMG`。
4. 解包 `app.sqsh4`，确认包内 `language.xls`、`sat_leo.ttf` 与 SX `res/` 源文件一致。
5. 从 `ME_AHD_ANYKA.IMG` 抽取 app 分区，与 `platform/app.sqsh4` 对比一致。
6. 设备启动后查看日志：

```text
[language_xls] init success
[language] set id=... col=... xls=1 ...
```
