# 90-Degree Clockwise Rotation of 24/32-bit BMP Image

可执行文件 `rotatebmp` 在 macOS 下编译，编译命令为 `g++ rotatebmp.cpp -o rotatebmp -std=c++17`

可执行文件 `rotatebmp.exe` 在 win11 下编译，编译命令为 `g++ rotatebmp.cpp -o rotatebmp -std=c++17`

本项目的源代码为 `rotatebmp.cpp`

可以通过 `rotatebmp src dest` 实现 24/32-bit BMP 图像顺时针旋转 90 度，具体效果是从 `src` 变成 `dest`

使用时应保证 `src` 为 24/32-bit BMP 图像，且无压缩

如果见到 `[Finished]Rotated successfully` 说明旋转成功

|  错误信息 | 错误内容  | 
| -                                     | - | 
| `[Error] Unable to open input file`   | 无法打开 `src`                                        | 
| `[Error] Unable to open output file`  | 无法打开 `dest`                                       | 
| `[Error] Invalid input type`          | `src` 不是 BMP 文件                                   | 
| `[Error] Not 24-bit or 32-bit BMP`    | `src` 是 BMP 文件，但不是 24-bit 或者 32-bit，无法处理   | 
| `[Error] Invalid infoheader`          | `src` 的信息头有问题                                   |

实现思路：

`BMPHeader` 用于存储文件头

`BMPInfoHeader` 用于存储信息头，保留了旋转图像所需要的信息，事实上就是 V3 信息头

读入 文件头 和 信息头 后，通过 `vector<uint8_t>` 来存储像素信息

函数 `rotateClockwise90` 实现了顺时针旋转 90 度，其中的 $(i,j)$ 表示的是图像上的坐标，而非存储的坐标

最后输出时，通过 `infoHeader.biSize` 将额外的色彩信息（V4/V5 信息头）复制过来
