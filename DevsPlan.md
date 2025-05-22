### 构建系统
- [ ] CMake 构建
- [ ] Stellatus 的构建方案 -> 参考Blender 的构建系统
- [ ] setup.sh & setup.bat

### 库的实验方案
- [ ] OrderedList 的实现 -> 参考标准库的 xtree 的实现
- [ ] Math 库
  - [ ] 重新构建 Vector
  - [ ] 完成矩阵库
- [ ] 二维数组封装: 二维数组不等于矩阵
- [ ] String 工具库的实现
  - [ ] 回文串算法
  - [ ] Python风格String API

### 核心功能
- [ ] 渲染RHI的封装
  - [ ] 跑通 Vulkan 流程
  - [ ] 跑通 OpenGL 流程
- [ ] 多exe的通信方案 (多平台封装)
  - [ ] 共享内存的方案
  - [ ] Socket 方案
- [ ] UI界面接入
  - [ ] SDL3, SDL_Image etc.
  - [ ] Imgui
- [ ] 文件读取器
- [ ] 资产导入器

### 第三方库的接入
- [ ] re2: 高性能正则表达式库
- [ ] ufbx: fbx 格式导入库
- [ ] glm: 图形学矩阵库