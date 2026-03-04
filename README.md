```
Usage: ./Tool.sh [option]
Options:
  make       创建build目录并构建项目
  clean      删除build目录及内容
  completion 配置自动补全功能（用户级，永久生效）

make output -> bin
```

---
#Config参数
```
string Config = R"(
PC_IP: 192.168.3.245
SN: 12345678912345678
Boards:
  - Id: 101
    IP: 192.168.3.105
    # Port: 19001
)";

    string ADDR = dir_path.string() + "/../config/YAML/New_Test/out/TOP.yaml";
    if(Test_Robot->Init_TOP(ADDR, Config) != 0)
    { 
        cout << "Init_ERR" << endl;
        return -1;
    }
```
```
Config 文本为YAML文件格式
PC_IP: 为电脑的IP地址 此地址网段要和下面配置的IP网段一致
SN: 产品SN码
Boards: 一个产品可能有多个板卡 使用 Id 区分 每个板卡的SN码一致
Id: 板卡id 此id号列表相见对应产品demo或者产品手册
IP: 板卡ip ip要保证和PC_IP在同一网段 同时不能重复
Port: 端口号 默认不使用 SDK会根据ip自动推算 如果发现重复可以解开注释人工配置
```

