// 引入pybind11核心头文件（安装后pip会自动配置路径，无需手动指定）
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // 用于C++容器（vector/map）与Python列表/字典的自动转换
#include <string>
#include <vector>
#include <iostream> 
using namespace std;

// 命名空间简写，简化代码
namespace py = pybind11;

// 1. 简单的无参无返回值C++函数
void hello_world() {
    std::cout << "Hello from C++! 这是Python调用的C++函数" << std::endl;
}

// 2. 带参数+返回值的C++函数（int类型）
int add(int a, int b) {
    return a + b;  // 计算效率远高于Python的加法
}

// 3. 字符串类型的函数（C++ string ↔ Python str）
std::string greet(const std::string& name) {
    return "Hello, " + name + "! 来自C++的问候";
}

// 4. 容器类型（C++ vector ↔ Python list）
std::vector<int> multiply_list(const std::vector<int>& nums, int factor) {
    std::vector<int> res;
    for (int n : nums) {
        res.push_back(n * factor);
    }
    return res;
}

// 5. 导出C++类到Python（Python可直接实例化、调用成员函数/属性）
class Calculator {
private:
    int base;  // 私有成员
public:
    typedef struct 
    {
        int a;
        int b;
    }acc;
public:
    // 构造函数
    Calculator(int b) : base(b) {}

    // 成员函数：累加base
    int add_base(int x) {
        base += x;
        return base;
    }

    // 成员函数：创建并返回acc结构体对象
    acc get_base(int a, int b) {
        acc accs;
        accs.a=a;
        accs.b=b;
        return accs;
    }

    // 静态成员函数：计算acc结构体中a+b
    static int static_add(acc data) {
        return data.a + data.b;
    }
};

// 核心：PYBIND11_MODULE宏，将C++代码导出为Python模块
// 格式：PYBIND11_MODULE(模块名, 模块对象)
// 注意：模块名必须与后续Python导入的名字一致，且不能有空格
PYBIND11_MODULE(cpp_py_lib, m) {
    // 模块文档字符串（Python中执行help(cpp_py_lib)可查看）
    m.doc() = "pybind11实现的C++转Python扩展库示例";

    // 导出普通函数：def("Python中调用的函数名", &C++函数名, "函数文档字符串")
    m.def("hello_world", &hello_world, "输出Hello World（无参无返回）");
    m.def("add", &add, "两个整数相加，参数：a(int), b(int)，返回：a+b(int)");
    m.def("greet", &greet, "字符串问候，参数：name(str)，返回：问候语(str)");
    m.def("multiply_list", &multiply_list, "列表元素乘系数，参数：nums(list), factor(int)，返回：新列表(list)");

    // 关键修改1：注册嵌套结构体Calculator::acc
    // 模板参数指定嵌套结构体：Calculator::acc
    // Python中结构体名为Acc（规范命名，也可直接用acc）
    // def_readwrite 导出结构体的公有成员变量，支持Python读写
    py::class_<Calculator::acc>(m, "Acc", "Calculator的嵌套结构体，包含两个int成员a/b")
        .def_readwrite("a", &Calculator::acc::a, "整型成员a")
        .def_readwrite("b", &Calculator::acc::b, "整型成员b");

    // 关键修改2：先注册依赖的Acc结构体，再注册Calculator类（顺序不可反）
    // 导出C++类：class_<C++类名>(模块对象, "Python中类名", "类文档字符串")
    py::class_<Calculator>(m, "Calculator", "一个简单的计算器类，维护一个基础值base")
        .def(py::init<int>(), "构造函数，参数：b(int)（初始化base值）")  // 导出构造函数
        .def("add_base", &Calculator::add_base, "累加base，参数：x(int)，返回：新base(int)")  // 导出成员函数
        // 修正文档字符串：原文档与实际功能不符
        .def("get_base", &Calculator::get_base, "创建Acc结构体对象，参数：a(int), b(int)，返回：Acc对象")
        // 静态函数参数为Acc，pybind11已识别注册的类型
        .def_static("static_add", &Calculator::static_add, "静态函数：计算Acc对象的a+b，参数：data(Acc)，返回：a+b(int)");
}
