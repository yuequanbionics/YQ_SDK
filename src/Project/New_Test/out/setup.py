# 导入必要的构建工具
from setuptools import setup, Extension
from pybind11.setup_helpers import Pybind11Extension, build_ext # type: ignore
import sys

# 获取Python的版本号（确保编译的库与Python版本兼容）
__version__ = "0.0.1"

# 配置C++扩展模块
# 核心：Pybind11Extension（pybind11封装的扩展类，自动处理编译参数/链接）
# 参数说明：
# 1. 模块名：必须与C++代码中PYBIND11_MODULE的第一个参数完全一致（示例中是cpp_py_lib）
# 2. sources：C++源代码文件路径（多个文件用列表，如["a.cpp", "b.cpp"]）
# 3. extra_compile_args：额外的C++编译参数，指定C++标准（必须≥C++11，pybind11要求）
ext_modules = [
    Pybind11Extension(
        "cpp_py_lib",
        sources=["my_cpp_code.cpp"],
        extra_compile_args=["-std=c++11"] if sys.platform != "win32" else ["/std:c++11"],
        define_macros=[("VERSION_INFO", __version__)],
    ),
]

# 执行setup：构建并打包C++扩展库
setup(
    name="cpp_py_lib",  # 库的名字（pip install时的名字，可自定义）
    version=__version__,
    author="Your Name",
    author_email="your_email@example.com",
    description="C++转Python扩展库示例",
    long_description="",
    ext_modules=ext_modules,  # 关联上面的扩展模块
    cmdclass={"build_ext": build_ext},  # 指定构建命令为pybind11的build_ext
    zip_safe=False,  # 扩展库不能压缩，必须设为False
    python_requires=">=3.8",  # 支持的Python版本
)
