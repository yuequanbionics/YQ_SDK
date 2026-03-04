python setup.py build_ext --inplace

    build_ext：表示构建扩展库；
    --inplace：表示将编译后的库生成到当前目录（方便直接调用，否则会生成在 build 文件夹中）。

# 安装wheel包工具
pip install wheel
# 编译wheel包（生成在dist文件夹中）
python setup.py bdist_wheel
# 他人安装时，直接执行
pip install dist/cpp_py_lib-0.0.1-cp310-cp310-win_amd64.whl

pip list


进阶
2. 导出 C++ 的常量 / 枚举
cpp
运行

// C++代码中添加
m.attr("PI") = 3.1415926;  // 导出常量
// 导出枚举
enum class Color { Red, Green, Blue };
py::enum_<Color>(m, "Color")
    .value("Red", Color::Red)
    .value("Green", Color::Green)
    .value("Blue", Color::Blue)
    .export_values();

Python 中可直接调用：print(cpp_py_lib.PI)、cpp_py_lib.Color.Red。
3. 处理 Python 的关键字参数
C++ 函数导出时可指定关键字参数，方便 Python 调用：
cpp
运行

// 原add函数改为支持关键字参数
m.def("add", &add, "两个整数相加", py::arg("a"), py::arg("b"));

Python 中可这样调用：cpp_py_lib.add(a=10, b=20)。

















