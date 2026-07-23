# 云山湖景区地图系统

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Language](https://img.shields.io/badge/language-C%20(C17)-green.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Build](https://img.shields.io/badge/build-CMake-blue.svg)](CMakeLists.txt)

一个基于 C 语言 + CMake 的终端景区地图系统，支持 ASCII 地图展示、景点查询、最短路径导航（Dijkstra）与推荐游览路线（最近邻启发式）。

## 功能

1. **查看景区地图** —— 终端绘制 60×18 ASCII 地图，标注景点与步道/公路
2. **景点列表与详情** —— 列出全部景点，按编号查看名称、类别、坐标、简介
3. **最短路径导航** —— Dijkstra 算法计算两景点间最短步行路径，并在地图上高亮
4. **推荐游览路线** —— 最近邻启发式生成游览顺序，输出总距离与预估时长，地图高亮整条路线
5. **重新加载数据** —— 修改数据文件后无需重启即可重新载入

## 目录结构

```
.
├── CMakeLists.txt
├── data/
│   ├── spots.txt      # 景点数据
│   └── paths.txt      # 道路数据
├── src/
│   ├── main.c         # 主循环与菜单
│   ├── data.{h,c}     # 数据结构、文件加载、邻接矩阵
│   ├── map.{h,c}      # ASCII 地图绘制与高亮
│   ├── graph.{h,c}    # Dijkstra 最短路径
│   ├── route.{h,c}    # 推荐游览路线
│   └── ui.{h,c}       # 终端 UI 辅助
└── README.md
```

## 构建与运行

需要 CMake 3.10+ 与支持 C17 的编译器（MSVC / GCC / Clang 均可）。

```bash
# 在项目根目录下
cmake -S . -B build
cmake --build build --config Debug
```

运行：

```bash
# Windows
build\Debug\scenic_map.exe      # MSVC 多配置生成器
# 或
build\scenic_map.exe            # 单配置生成器（如 Ninja）

# Linux / macOS
./build/scenic_map
```

> 程序启动时会依次尝试 `data/spots.txt`、`../data/spots.txt`、`../../data/spots.txt`，
> 因此在项目根目录、`build/` 或 `build/Debug/` 下运行均可找到数据。
> 构建时 CMake 已自动把 `data/` 拷贝到可执行文件目录。

## 数据文件格式

### `data/spots.txt`

以 `#` 开头为注释，字段以空白分隔，描述为行尾剩余部分（可含空格）：

```
# id  名称  类别  x  y  描述
1  游客中心  休息区  4  14  景区主入口，提供导览图与票务服务
```

坐标 `x∈[1,58]`（列）、`y∈[1,16]`（行），对应 60×18 地图内部。

### `data/paths.txt`

```
# from  to  distance(米)  type
1  2  600  步道
```

`type` 为 `步道` 或 `公路`（在地图上分别显示为 `.` 与 `:`）。图为无向图，重边取较短者。

## 地图图例

| 字符 | 含义 |
|------|------|
| `1`-`9`,`a`-`z` | 景点标记 |
| `.` | 步道 |
| `:` | 公路 |
| `*` | 高亮路径（红色） |
| `#` | 地图边框 |

## 算法说明

- **最短路径**：O(V²) Dijkstra，使用 `dist[]`/`prev[]` 数组，回溯前驱还原途经序列。
- **推荐路线**：最近邻启发式求解类 TSP——从起点出发，每步走到「Dijkstra 距离最短的未访问景点」。预估时长按步行 70 米/分钟计算。

## 扩展

新增景点/道路只需编辑 `data/` 下两个文本文件，菜单 5 重新加载即可生效，无需重新编译。
