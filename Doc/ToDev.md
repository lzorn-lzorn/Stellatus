# Design
../Engine/Include: 引擎头文件
../Engine/Source:  引擎源码
../Engine/Lib:     **第三方库**

对于 Include 和 Source 中的模块, 他们往往会有比较强的层级依赖关系:
Editor -> Core -> Platfrom
Editor: 是UI层负责各个窗口的显示
Core: 是核心层, 相当于后端
../Bin/Win:   Windows 平台下的输出路径
../Bin/Linux: Linux   平台下的输出路径
../Bin/IOS:   IOS     平台下的输出路径
## 构建
CMake 变量说明

## 使用组件
### Math 库
Math 库是指代一个 MathAdapter 其内部封装了各种第三方的数学库, 上层的调用则只需要调用统一的结构

