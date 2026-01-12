#!/bin/bash

# 获取当前脚本的绝对路径(无论从哪里调用)
SCRIPT_DIR=$(dirname "$(readlink -f "$0")")

# --------------------------
# 自动补全函数定义
# --------------------------
_tool_completion() {
    local commands=("make" "clean" "Dmake" "init_install" "completion" "menu_config" "ipconfig" "set_id" "make_config" "make_third" "build_out")  
    local cur=${COMP_WORDS[COMP_CWORD]}
    if [ $COMP_CWORD -eq 1 ]; then
        COMPREPLY=($(compgen -W "${commands[*]}" -- "$cur"))
    else
        COMPREPLY=()
    fi
}

# --------------------------
# 自动绑定补全函数(交互模式下)
# --------------------------
if [ -n "$BASH_VERSION" ] && [[ $- == *i* ]]; then
    script_path=$(realpath "$0")
    # 统一使用 _tool_completion 函数名,避免特殊字符
    complete -F _tool_completion "$script_path"
    complete -F _tool_completion "$(basename "$script_path")"
fi

# --------------------------
# 区分脚本运行方式(source/直接执行)
# --------------------------
if [[ "${BASH_SOURCE[0]}" != "${0}" ]]; then
    return 0
fi

# --------------------------
# 帮助信息函数
# --------------------------
usage() {
    echo "Usage: $0 [option]"
    echo "Options:"
    echo "  make         创建build目录并构建项目"
    echo "  clean        删除build目录及内容"
    echo "  Dmake        Docker环境下编译"
    echo "  init_install 初始化安装"
    echo "  autorun      配置自启动"
    echo "  completion   配置自动补全功能(用户级,永久生效)"
    echo "  menu_config  启动配置菜单)" 
    echo "  make_config  切换编译配置文件" 
    echo "  make_third   编译第三方库" 
    echo "  ipconfig     设置网卡IP"  
    echo "  set_id       设置设备ID"  
    echo "  build_out    构建输出版"  
    exit 1
}

# --------------------------
# 参数检查
# --------------------------
if [ $# -eq 0 ]; then
    usage
fi

make_config_F() {
    echo "===== 启动配置菜单 ====="
    local config_dir="config/make_config"
    mkdir -p "$config_dir"  # 确保配置目录存在

    echo -e "\n请选择操作:"
    echo "1. 设置 make_config(从模板文件加载)"
    echo "2. 保存 make_config(当前配置存为模板)"
    echo "q. 退出配置菜单"
    read -p "请输入选项(1/2/q):" choice

    case "$choice" in
        1)
            echo -e "\n===== 加载模板文件 ====="
            shopt -s nullglob
            local txt_files=("$config_dir"/*.txt)
            shopt -u nullglob

            if [ ${#txt_files[@]} -eq 0 ]; then
                echo "提示:$config_dir 目录下未找到任何 .txt 模板文件"
                return  # 无循环，直接返回
            fi

            echo "可用模板文件列表:"
            for i in "${!txt_files[@]}"; do
                local filename=$(basename "${txt_files[$i]}")
                echo "$((i+1)). $filename"
            done

            read -p "请输入模板标号(1-${#txt_files[@]}):" num
            if ! [[ "$num" =~ ^[0-9]+$ ]] || [ "$num" -lt 1 ] || [ "$num" -gt "${#txt_files[@]}" ]; then
                echo "错误:无效的标号，请输入 1 到 ${#txt_files[@]} 之间的数字"
                return
            fi

            local selected_file="${txt_files[$((num-1))]}"
            cp "$selected_file" "./config.txt" && \
                echo "成功:已将 $(basename "$selected_file") 加载为当前配置(./config.txt)" || \
                echo "错误:文件拷贝失败"
            ;;

        2)
            echo -e "\n===== 保存当前配置 ====="
            if [ ! -f "./config.txt" ]; then
                echo "错误:当前目录下未找到 config.txt 文件，无法保存"
                return
            fi

            # 列出所有已存在的模板文件并标号
            shopt -s nullglob
            local txt_files=("$config_dir"/*.txt)
            shopt -u nullglob

            echo "当前已存在的模板文件:"
            if [ ${#txt_files[@]} -eq 0 ]; then
                echo "  无已保存的模板文件"
            else
                for i in "${!txt_files[@]}"; do
                    local filename=$(basename "${txt_files[$i]}")
                    echo "  $((i+1)). $filename"
                done
            fi

            # 读取用户输入(数字表示覆盖已有文件，非数字表示新文件)
            read -p "请输入保存名称或现有文件标号(数字):" save_input

            # 判断输入是否为数字
            if [[ "$save_input" =~ ^[0-9]+$ ]]; then
                # 输入为数字:处理覆盖已有文件逻辑
                local num=$save_input
                # 检查标号有效性
                if [ ${#txt_files[@]} -eq 0 ] || [ "$num" -lt 1 ] || [ "$num" -gt ${#txt_files[@]} ]; then
                    echo "错误:无效的标号，当前有效范围是 1-${#txt_files[@]}"
                    return
                fi
                # 获取目标文件路径
                local target_file="${txt_files[$((num-1))]}"
                local target_name=$(basename "$target_file")
                # 二次确认覆盖
                read -p "确认覆盖现有文件 '$target_name' 吗？(y/n):" overwrite
                if [ "$overwrite" != "y" ] && [ "$overwrite" != "Y" ]; then
                    echo "已取消保存"
                    return
                fi
                # 执行覆盖保存
                cp "./config.txt" "$target_file" && \
                    echo "成功:已覆盖保存为 $target_file" || \
                    echo "错误:文件保存失败"

            else
                # 输入为非数字:处理新文件保存逻辑
                local save_name="$save_input"
                if [ -z "$save_name" ]; then
                    echo "错误:保存名称不能为空"
                    return
                fi
                local target_file="$config_dir/$save_name.txt"
                # 直接保存(即使文件已存在也覆盖，无确认)
                cp "./config.txt" "$target_file" && \
                    echo "成功:已保存为 $target_file" || \
                    echo "错误:文件保存失败"
            fi
            ;;

        q|Q)
            echo -e "\n===== 配置菜单已退出 ====="
            ;;

        *)
            echo "错误:无效选项，请输入 1、2 或 q(退出)"
            ;;
    esac 
}

# --------------------------
# 主命令处理逻辑
# --------------------------
case "$1" in
    make)
        echo "===== 开始构建项目 ====="
        
        # 检查配置文件是否存在,不存在则调用配置菜单
        if [ ! -f "config.txt" ]; then
            echo "未找到配置文件 config.txt,启动配置菜单..."

            ARCH=$(uname -m)
            # 定义不同架构对应的执行路径
            if [ "$ARCH" = "x86_64" ]; then
                menu_config_path=./bin/Tool/x86/menu_config
                export TERMINFO="$SCRIPT_DIR/third_party/lib/install-x86_64/share/terminfo"
            elif [ "$ARCH" = "aarch64" ]; then
                menu_config_path=./bin/Tool/arm64/menu_config
                export TERMINFO="$SCRIPT_DIR/third_party/lib/install-arm64/share/terminfo"
            else
                echo "错误:不支持的架构 $ARCH"
                exit 1
            fi
            
            # 检查配置工具是否存在且可执行
            if [ ! -f $menu_config_path ]; then
                echo "错误:未找到 $menu_config_path 文件"
                exit 1
            fi
            if [ ! -x $menu_config_path ]; then
                echo "错误: $menu_config_path 没有执行权限,尝试添加权限..."
                chmod +x $menu_config_path || {
                    echo "添加执行权限失败,请手动执行 chmod +x $menu_config_path"
                    exit 1
                }
            fi
            
            # 执行配置菜单
            $menu_config_path
            
            # 检查配置后文件是否生成
            if [ ! -f "config.txt" ]; then
                echo "错误:配置菜单执行后仍未生成 config.txt 文件"
                exit 1
            fi
        fi

        cd third_party
        ./make_build.sh 
        cd ..
        
        if [ ! -d "build" ]; then
            echo "创建build目录..."
            mkdir build || { echo "创建build目录失败!"; exit 1; }
        fi
        cd build || { echo "无法进入build目录!"; exit 1; }
        cmake .. || { echo "CMake配置失败!"; exit 1; }
        make -j$(nproc) || { echo "编译失败!"; exit 1; }
        cd ..
        echo "===== 项目构建完成 ====="
        ;;

    clean)
        echo "===== 开始清理项目 ====="
        if [ -d "build" ]; then
            echo "删除build目录..."
            rm -rf build || { echo "删除build目录失败!"; exit 1; }
        else
            echo "build目录不存在,无需清理"
        fi
        echo "===== 项目清理完成 ====="
        ;;

    Dmake)
        echo "===== 开始构建项目 ====="
        # rm -rf lib
        # mkdir lib 

        # 显示版本选择菜单
        echo "请选择Ubuntu系统版本:"
        echo "1) Ubuntu 20.04"
        echo "2) Ubuntu 22.04"
        echo "3) Ubuntu 24.04"
        
        # 读取用户输入
        read -p "请输入选项(1-3):" choice

        # 映射选择到版本号
        case $choice in
            1)
                UBUNTU_VERSION="20.04"
                ;;
            2)
                UBUNTU_VERSION="22.04"
                ;;
            3)
                UBUNTU_VERSION="24.04"
                ;;
            *)
                # 处理无效输入
                echo "错误:无效的选项,请输入1、2或3"
                exit 1
                ;;
        esac

        # 显示选择的版本并执行构建脚本(传递版本参数)
        echo "你选择了Ubuntu $UBUNTU_VERSION,开始构建..."
        ./script/shell/Docker_Make.sh $UBUNTU_VERSION
        ;;

    init_install)
        echo "===== Init_安装依赖 ====="
        # 显示版本选择菜单
        echo "请输入选择:"
        echo "0) 鱼香ROS脚本"
        echo "1) Docker 安装"
        echo "2) Miniconda 安装"
        echo "3) Ollama 安装"
        
        # 读取用户输入
        read -p "请输入选项(0-3):" choice

        # 映射选择到版本号
        case $choice in
            0)
                wget http://fishros.com/install -O fishros && . fishros
                ;;
            1)
                script/shell/Init_Install_Docker.sh
                ;;
            2)
                sudo apt-get install -y git-lfs
                cd ./third_party
                git clone https://www.modelscope.cn/tokolll/conda.git
                cd ./conda
                git lfs install
                git config lfs.concurrenttransfers 10
                git lfs pull

                if [ ! -d "$HOME/miniconda3" ]; then
                    # 获取当前系统架构(x86_64 或 aarch64)
                    ARCH=$(uname -m)
                    # 定义不同架构对应的执行路径
                    if [ "$ARCH" = "x86_64" ]; then
                        bash ./miniconda.sh -b -u -p ~/miniconda3
                    elif [ "$ARCH" = "aarch64" ]; then
                        bash ./miniconda-aarch64.sh -b -u -p ~/miniconda3
                    else
                        echo "错误:不支持的架构 $ARCH"
                        exit 1
                    fi
                    source $HOME/miniconda3/bin/activate
                    source "$HOME/.bashrc"

                    echo "接受Anaconda服务条款..."
                    conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/main
                    conda tos accept --override-channels --channel https://repo.anaconda.com/pkgs/r

                    echo "初始化Conda环境..."
                    conda init --all

                    echo "重新加载环境配置..."
                    source "$HOME/.bashrc"
                else
                    echo "~/miniconda3 已存在,跳过安装"
                fi

                if ! conda env list | grep -q "sdk"; then
                    echo "创建sdk环境(Python 3.10)..."
                    conda create -n sdk python=3.10 -y
                    
                    cd ../..
                    echo "激活sdk环境并安装依赖..."
                    conda activate sdk
                    pip install -r script/python/requirements.txt
                else
                    echo "sdk环境已存在,跳过创建"
                fi

                echo "请人工执行此命令:  source ~/.bashrc"
                echo "所有操作完成!可通过 'conda activate sdk' 进入环境"
                ;;
            3)
                sudo apt-get install -y git-lfs
                cd ./third_party
                git clone https://www.modelscope.cn/tokolll/ollama.git
                cd ./ollama
                git lfs install
                git config lfs.concurrenttransfers 10
                git lfs pull

                if [ ! -f "/usr/bin/ollama" ]; then
                    sudo tar -C /usr -xzf ollama-linux-amd64.tgz

                    sudo useradd -r -s /bin/false -U -m -d /usr/share/ollama ollama
                    sudo usermod -a -G ollama $(whoami)

                    ollama --version

                    # 创建服务文件内容
                    SERVICE_CONTENT="[Unit]
Description=Ollama Service
After=network-online.target
 
[Service]
Environment="OLLAMA_HOST=0.0.0.0"
Environment="OLLAMA_ORIGINS=*"
ExecStart=/usr/bin/ollama serve
User=ollama 
Group=ollama 
Restart=always
RestartSec=3
 
[Install]
WantedBy=default.target"

                    sudo systemctl daemon-reload
                    sudo systemctl start ollama
                    sudo systemctl enable ollama
                    sudo systemctl status ollama
                    # ollama run SimonPu/Qwen3-Coder:30B-Instruct_Q4_K_XL
                fi
                ;;
            *)
                # 处理无效输入
                echo "错误:无效的选项,请输入0-3"
                exit 1
                ;;
        esac
        ;;

    autorun)
        echo "===== 自动RUN ====="
        # 显示功能菜单
        echo "请输入选择:"
        echo "1) 启用自动运行"
        echo "0) 禁用自动运行"
        
        # 读取用户输入
        read -p "请输入选项(1或0):" choice

        # 处理用户选择
        case $choice in
            1)
                script/shell/autorun/init_autorun.sh true
                ;;
            0)
                script/shell/autorun/init_autorun.sh false
                ;;
            *)
                # 处理无效输入
                echo "错误:无效的选项,请输入 1 或 0"
                exit 1
                ;;
        esac
        ;;

    ipconfig)
        ./script/shell/ipconfig.sh
        ;;

    set_id)
        ARCH=$(uname -m)
        # 第一步:让用户选择操作类型(1=switch,2=slave)
        while true; do
            echo -e "请选择操作设备类型:"
            echo "1 - 配置 Switch Board"
            echo "2 - 配置 Can Slave"
            read -p "请输入选择(1/2):" type_choice

            # 校验输入是否为 1 或 2
            if [ "$type_choice" = "1" ]; then
                func_type="switch"  # 对应程序需要的 switch 参数
                break
            elif [ "$type_choice" = "2" ]; then
                func_type="slave"   # 对应程序需要的 slave 参数
                break
            else
                echo "❌ 输入错误!仅支持输入 1 或 2,请重新选择。"
            fi
        done

        # 第二步:让用户输入 ID(校验范围:0~1020,匹配 u16 类型)
        while true; do
            read -p "请输入 $func_type 对应的 ID(范围:0~1020):" id_input

            # 1. 先校验是否为纯数字
            if ! [[ "$id_input" =~ ^[0-9]+$ ]]; then
                echo "❌ 输入错误!ID 必须是整数,请重新输入。"
                continue
            fi

            # 2. 再校验数字范围(0~1020)
            id_val=$((id_input))
            if [ $id_val -ge 0 ] && [ $id_val -le 1020 ]; then
                break  # ID 合法,退出循环
            else
                echo "❌ 输入错误!ID 必须在 0~1020 之间,请重新输入。"
            fi
        done

        # 第三步:根据架构调用对应的 Set_ID 程序,并传入参数
        echo -e "\n✅ 准备执行:配置 $func_type,ID=$id_val,架构=$ARCH"
        if [ "$ARCH" = "x86_64" ]; then
            ./bin/Tool/x86/Set_ID "$func_type" "$id_val"
        elif [ "$ARCH" = "aarch64" ]; then
            ./bin/Tool/arm64/Set_ID "$func_type" "$id_val"
        else
            echo "❌ 错误:不支持的架构 $ARCH"
            exit 1
        fi

        # 检查程序执行结果,提示用户是否成功
        if [ $? -eq 0 ]; then
            echo -e "\n✅ $func_type 配置完成!"
        else
            echo -e "\n❌ $func_type 配置失败,请检查设备连接或参数是否正确。"
            exit 1
        fi
        ;;

    completion)
        echo "===== 配置自动补全 ====="
        
        script_path=$(realpath "$0")
        script_name=$(basename "$script_path")
        bashrc_file=~/.bashrc
        
        echo "添加补全配置到 ${bashrc_file}..."
        
        # 清理所有旧的补全绑定和配置
        echo "清理旧的补全绑定..."
        complete -r "$script_name" 2>/dev/null
        complete -r "./$script_name" 2>/dev/null
        complete -r "$script_path" 2>/dev/null
        
        echo "清理旧的补全配置..."
        # 1. 删除旧的函数定义(精确匹配函数边界)
        sed -i '/^_tool_completion() {$/,/^}$/d' "$bashrc_file"
        
        # 2. 删除所有相关的 complete 命令(仅匹配以当前脚本名结尾的)
        sed -i "/complete -F _tool_completion.*${script_name}$/d" "$bashrc_file"
        
        # 3. 删除相关注释(使用唯一标记)
        sed -i "/# ${script_name} 自动补全配置.*$/d" "$bashrc_file"
        

        # 写入新配置(添加注释标记,便于后续清理)
cat >> "$bashrc_file" <<EOF
# ${script_name} 自动补全配置
_tool_completion() {
    local commands=("make" "clean" "Dmake" "init_install" "completion" "menu_config" "ipconfig" "set_id" "make_config" "make_third" "build_out")  
    local cur=\${COMP_WORDS[COMP_CWORD]}
    if [ \$COMP_CWORD -eq 1 ]; then
        COMPREPLY=(\$(compgen -W "\${commands[*]}" -- "\$cur"))
    else
        COMPREPLY=()
    fi
}
complete -F _tool_completion ${script_path}
complete -F _tool_completion ${script_name}
complete -F _tool_completion ./${script_name}
EOF
        echo "✅ 自动补全配置已写入 ${bashrc_file}!"
        echo "请执行以下命令使配置立即生效:"
        echo "  source ${bashrc_file}"
        echo "或新开一个终端会话。"
        ;;

    menu_config)  # 新增 menu_config 命令处理
        echo "===== 启动配置菜单 ====="
        
        # 获取当前系统架构(x86_64 或 aarch64)
        ARCH=$(uname -m)
        # 定义不同架构对应的执行路径
        if [ "$ARCH" = "x86_64" ]; then
            MENU_PATH="./bin/Tool/x86/menu_config"
            export TERMINFO="$SCRIPT_DIR/third_party/lib/install-x86_64/share/terminfo"
        elif [ "$ARCH" = "aarch64" ]; then
            MENU_PATH="./bin/Tool/arm64/menu_config"
            export TERMINFO="$SCRIPT_DIR/third_party/lib/install-arm64/share/terminfo"
        else
            echo "错误:不支持的架构 $ARCH"
            exit 1
        fi

        # 检查文件是否存在且可执行
        if [ ! -f "$MENU_PATH" ]; then
            echo "错误:未找到 $MENU_PATH 文件"
            exit 1
        fi
        if [ ! -x "$MENU_PATH" ]; then
            echo "错误:$MENU_PATH 没有执行权限,尝试添加权限..."
            chmod +x "$MENU_PATH" || {
                echo "添加执行权限失败,请手动执行 chmod +x $MENU_PATH"
                exit 1
            }
        fi

        # 执行对应架构的配置菜单
        $MENU_PATH
        echo "===== 配置菜单已退出 ====="
        ;;

    make_config) 
        make_config_F
        ;;

    make_third) 
        cd third_party
        ./make_build.sh make
        cd ..
        ;;

    build_out)
        if ! command -v conda &> /dev/null; then
            echo -e "\033[31m错误:未检测到 Conda 安装！\033[0m"
            echo -e "请先安装 Conda"
            exit 1  # 退出脚本，终止后续操作
        fi

        if ! conda env list | grep -qE '^sdk\s+'; then
            echo -e "\033[31m错误:未检测到 Conda 环境 'sdk'!\033[0m"
            echo -e "请先创建 sdk 环境"
            echo -e "创建完成后重新运行脚本"
            exit 1  # 退出脚本，终止后续操作
        fi

        echo -e "\033[32m✅ Conda 及 sdk 环境检测通过，开始执行 build_out 操作...\033[0m"
        
        conda run -n sdk --no-capture-output python ./script/python/Tool/build_out.py
        ;;

    *)
        echo "错误:未知命令 '$1'"
        usage
        ;;
esac
