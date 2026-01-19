from pathlib import Path
import shutil
import os
import re
import sys


current_script_path = os.path.abspath(__file__)
current_script_dir = os.path.dirname(os.path.abspath(__file__))
current_script_dir = current_script_dir.split("/script/python/Tool")[0]
current_script_dir_out = current_script_dir + "_out"
current_script_dir_out_git = current_script_dir_out + "/.git"
current_script_dir_out_src = current_script_dir_out + "/src"
current_script_dir_out_config = current_script_dir_out + "/config"
current_script_dir_out_bin = current_script_dir_out + "/bin"
current_script_dir_out_inc = current_script_dir_out + "/inc"
current_script_dir_out_lib = current_script_dir_out + "/lib/ubuntu20.04/x86_64"
current_script_dir_out_Cmake = current_script_dir_out + "/CMakeLists.txt"

print("当前脚本路径:", current_script_path)
print("目录:", current_script_dir)
print("目录:", current_script_dir_out)

# 4. 处理目标目录：存在则删除，然后复制原目录到目标目录
def copy_directory_with_cleanup(src_dir, dst_dir):
    # 转换为 Path 对象（更安全的路径处理）
    src = Path(src_dir)
    dst = Path(dst_dir)
    
    # 检查源目录是否存在（避免复制不存在的目录）
    if not src.exists() or not src.is_dir():
        print(f"错误：源目录 {src} 不存在或不是目录！")
        return False
    
    # 若目标目录存在，先删除（递归删除整个目录树）
    if dst.exists():
        print(f"目标目录 {dst} 已存在，正在删除...")
        # Windows 下若有文件占用，可添加 ignore_errors=True 忽略（谨慎使用）
        shutil.rmtree(dst, ignore_errors=False)
    
    # 复制源目录到目标目录（递归复制所有文件和子目录）
    print(f"正在复制 {src} -> {dst}...")
    shutil.copytree(src, dst)
    print(f"复制完成！目标目录：{dst}")
    return True

# 执行复制逻辑
copy_directory_with_cleanup(current_script_dir, current_script_dir_out)
shutil.rmtree(current_script_dir_out_git, ignore_errors=False)
shutil.rmtree(current_script_dir_out_src + "/Tool", ignore_errors=False)
shutil.rmtree(current_script_dir_out + "/config/make_config", ignore_errors=False)
try:
    shutil.rmtree(current_script_dir_out + "/bin/logs", ignore_errors=False)
except:
    pass
try:
    shutil.rmtree(current_script_dir_out + "/build", ignore_errors=False)
except:
    pass
try:
    os.remove(current_script_dir_out + "/config.txt")
except:
    pass
shutil.copytree(current_script_dir_out + "/lib/ubuntu20.04", current_script_dir_out + "/lib", dirs_exist_ok=True)

def delete_top_level_files_only(target_dir):
    """
    删除目标目录下的所有顶层文件（非子目录），不递归处理子目录
    
    Args:
        target_dir (str/Path): 目标目录路径（current_script_dir_out_bin）
    """
    dir_path = Path(target_dir)
    
    # 检查目录是否存在
    if not dir_path.exists() or not dir_path.is_dir():
        print(f"❌ 错误：目录 {dir_path} 不存在或不是目录！")
        return
    
    print(f"ℹ️  开始处理目录：{dir_path}")
    print(f"ℹ️  仅删除顶层文件，不递归处理子目录...")
    
    # 遍历目录下的所有顶层条目（文件+子目录）
    deleted_count = 0
    for entry in dir_path.iterdir():
        # 只处理「文件」（排除子目录）
        if entry.is_file():
            try:
                entry.unlink()  # 删除单个文件
                print(f"✅ 已删除文件：{entry.name}")
                deleted_count += 1
            except PermissionError:
                print(f"⚠️  警告：权限不足，无法删除文件 {entry.name}")
            except Exception as e:
                print(f"⚠️  警告：删除文件 {entry.name} 失败 - {str(e)}")
        else:
            # 跳过子目录（不递归处理）
            print(f"ℹ️  跳过子目录：{entry.name}（不递归删除）")
    
    print(f"\n✅ 处理完成！共删除 {deleted_count} 个顶层文件")

delete_top_level_files_only(current_script_dir_out_bin)

# 存储结果的字典：key=唯一LIB_NAME，value=CMakeLists.txt所在目录（绝对路径字符串）
lib_cmake_dir_dict = {}

# 正则表达式：匹配 set(LIB_NAME XXXX)，其中XXXX是无空格的字符串
lib_name_pattern = re.compile(
    r'\s*set\s*\(\s*LIB_NAME\s+([a-zA-Z0-9_-]+)\s*\)\s*',
    re.IGNORECASE  # 忽略大小写（支持 set(LIB_NAME)、SET(LIB_NAME) 等写法）
)

def extract_lib_name_from_cmake(cmake_file):
    """从单个CMakeLists.txt文件中提取LIB_NAME值，存入字典（key=LIB_NAME，value=文件所在目录）"""
    try:
        # 获取当前CMakeLists.txt的所在目录（绝对路径）
        cmake_dir = cmake_file.parent.resolve()
        # 读取文件内容
        with open(cmake_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # 查找所有匹配的LIB_NAME
        matches = lib_name_pattern.findall(content)
        for lib_name in matches:
            # 去重：若LIB_NAME已存在，保留第一个对应的目录；若需覆盖可删除判断
            if lib_name not in lib_cmake_dir_dict:
                lib_cmake_dir_dict[lib_name] = str(cmake_dir)  # 转为字符串更易读
                print(f"找到LIB_NAME：{lib_name}（来自文件：{cmake_file}，所在目录：{cmake_dir}）")
            else:
                # 同一LIB_NAME对应多个目录时，输出警告（可根据需求调整为覆盖）
                print(f"警告：LIB_NAME {lib_name} 已存在（已存目录：{lib_cmake_dir_dict[lib_name]}），跳过当前文件：{cmake_file}")
    
    except Exception as e:
        print(f"警告：读取文件 {cmake_file} 时出错 - {str(e)}")

def recursive_find_cmake_and_extract_lib(dir_path):
    """递归遍历目录，查找所有CMakeLists.txt并提取LIB_NAME+对应目录"""
    # 检查目录是否存在
    if not dir_path.exists() or not dir_path.is_dir():
        print(f"错误：目录 {dir_path} 不存在或不是目录！")
        return
    
    print(f"开始递归遍历目录：{dir_path}")
    
    # 递归遍历所有子目录，查找CMakeLists.txt
    for cmake_file in dir_path.rglob("CMakeLists.txt"):  # rglob 递归匹配所有子目录
        extract_lib_name_from_cmake(cmake_file)

# 执行核心逻辑（确保 current_script_dir_out_src 已定义，若未定义可直接指定路径）
# 示例：current_script_dir_out_src = Path("/your/target/src_dir")  # 自定义路径
recursive_find_cmake_and_extract_lib(Path(current_script_dir_out_src))

# 输出最终结果
print("\n" + "="*60)
print(f"最终提取的 LIB_NAME-CMake目录 字典（共 {len(lib_cmake_dir_dict)} 个唯一LIB_NAME）：")
print("-" * 60)
for idx, (lib_name, cmake_dir) in enumerate(lib_cmake_dir_dict.items(), 1):
    print(f"{idx:2d}. LIB_NAME：{lib_name:<15} CMake目录：{cmake_dir}")
    shutil.rmtree(cmake_dir, ignore_errors=False)


target_dir = Path(current_script_dir_out_src)

def replace_depends_lines(config_file):
    """替换.config文件中含DEPENDS=或DEPENDS_=的行，添加#注释"""
    try:
        # 读取文件内容并处理替换
        with open(config_file, "r", encoding="utf-8") as f:
            lines = f.readlines()

        modified_lines = []
        replace_count = 0  # 统计替换次数

        for line in lines:
            # 提取原行的缩进部分（行首所有空白字符，包括空格、制表符）
            indent = ""
            for char in line:
                if char.isspace():
                    indent += char
                else:
                    break
            
            # 提取去除缩进后的内容部分（用于判断是否匹配目标行）
            content_part = line.lstrip()
            
            # 匹配 DEPENDS= 或 DEPENDS_= 开头的行
            if content_part.startswith("DEPENDS=") or content_part.startswith("DEPENDS_="):
                # 保留原缩进，在缩进后添加 # + 原内容（格式：缩进 + # + 原内容 + 换行）
                modified_line = f"{indent}# {content_part}"
                # 确保末尾保留换行符（与原行一致）
                if not modified_line.endswith("\n"):
                    modified_line += "\n"
                modified_lines.append(modified_line)
                replace_count += 1
                
                # 日志输出（简化显示，突出核心替换）
                original_line_display = line.strip()
                modified_line_display = modified_line.strip()
                print(f"替换文件 {config_file} 中的行：{original_line_display} -> {modified_line_display}")
            else:
                # 不匹配的行保持原样（保留所有缩进和格式）
                modified_lines.append(line)

        # 写入修改后的内容
        with open(config_file, "w", encoding="utf-8") as f:
            f.write("".join(modified_lines))

        if replace_count == 0:
            print(f"文件 {config_file} 中未找到 DEPENDS= 或 DEPENDS_= 相关行")
        else:
            print(f"文件 {config_file} 处理完成，共替换 {replace_count} 行")

    except Exception as e:
        print(f"警告：处理文件 {config_file} 时出错 - {str(e)}")

def recursive_find_config_files(dir_path):
    """递归遍历目录，查找所有.config文件并处理替换"""
    # 检查目录是否存在
    if not dir_path.exists() or not dir_path.is_dir():
        print(f"错误：目录 {dir_path} 不存在或不是目录！")
        return

    print(f"\n开始递归遍历目录：{dir_path}")
    # 递归查找所有 .config 文件（包括所有子目录）
    config_files = list(dir_path.rglob(".config"))  # rglob 递归匹配

    if not config_files:
        print(f"未在 {dir_path} 及其子目录中找到 .config 文件")
        return

    print(f"共找到 {len(config_files)} 个 .config 文件，开始处理...")
    for config_file in config_files:
        replace_depends_lines(config_file)

# 执行核心逻辑
recursive_find_config_files(target_dir)

print("\n" + "="*50)
print("所有 .config 文件处理完成！")

lib_out = []

def extract_so_lib_names(dir_path):
    """递归遍历目录，查找所有.so文件并提取目标名称"""
    # 检查目录是否存在
    if not dir_path.exists() or not dir_path.is_dir():
        print(f"错误：目录 {dir_path} 不存在或不是目录！")
        return
    
    print(f"\n开始递归遍历目录：{dir_path}")
    
    # 递归查找所有.so文件（包括所有子目录）
    # 匹配规则：文件名包含 .so（支持 .so、.so.1、.so.1.0.0 等版本后缀）
    for so_file in dir_path.rglob("*.so*"):
        # 只处理文件（排除目录，避免误匹配）
        if so_file.is_file():
            # 获取.so文件的文件名（不含路径）
            so_filename = so_file.name
            print(f"找到.so文件：{so_filename}（路径：{so_file}）")
            
            # 步骤1：去除前缀 "lib"（若存在）
            if so_filename.startswith("lib"):
                lib_name = so_filename[3:]  # 从第3个字符开始截取（跳过"lib"）
            else:
                lib_name = so_filename  # 若没有lib前缀，直接使用原文件名
            
            # 步骤2：去除后缀 ".so" 及后续版本号（如 .so.1.0）
            # 找到第一个 ".so" 的位置，截取前面的部分
            if ".so" in lib_name:
                lib_name = lib_name.split(".so")[0]
            
            # 步骤3：去重并添加到结果列表（避免重复）
            if lib_name and lib_name not in lib_out:
                lib_out.append(lib_name)
                print(f"提取成功：{lib_name}")

# 执行核心逻辑
extract_so_lib_names(Path(current_script_dir_out_lib))

# 输出最终结果
print("\n" + "="*50)
print(f"最终提取的库名称列表（lib_out）：")
for idx, lib in enumerate(lib_out, 1):
    print(f"{idx}. {lib}")
print(f"共提取到 {len(lib_out)} 个唯一库名称")


def insert_import_shared_library(cmake_file_path, lib_list):
    """
    在 CMake 文件的指定标记之间插入 import_shared_library(lib_name) 语句
    
    Args:
        cmake_file_path (str/Path): 目标 CMake 文件路径
        lib_list (list): 库名称列表（如 lib_out）
    """
    cmake_file = Path(cmake_file_path)
    
    # 检查文件是否存在
    if not cmake_file.exists() or not cmake_file.is_file():
        print(f"错误：文件 {cmake_file} 不存在或不是文件！")
        return False
    
    # 读取文件所有行（保留原始格式和换行）
    try:
        with open(cmake_file, "r", encoding="utf-8") as f:
            lines = f.readlines()
    except Exception as e:
        print(f"错误：读取文件 {cmake_file} 失败 - {str(e)}")
        return False
    
    # 定义标记行（精准匹配用户指定的字符串）
    start_mark = "# script auto start -> lib_add"
    end_mark = "# script auto end -> lib_add"
    
    # 查找开始标记和结束标记的索引
    start_idx = None
    end_idx = None
    for idx, line in enumerate(lines):
        # 去除行尾换行符后匹配（避免因换行符格式导致匹配失败）
        line_stripped = line.rstrip("\n\r")
        if line_stripped == start_mark:
            start_idx = idx
        elif line_stripped == end_mark:
            end_idx = idx
    
    # 检查标记是否存在且格式正确
    if start_idx is None or end_idx is None:
        print(f"错误：文件 {cmake_file} 中未找到完整标记（{start_mark} 和 {end_mark} 必须同时存在）")
        return False
    if start_idx >= end_idx:
        print(f"错误：标记顺序错误 - {start_mark} 必须在 {end_mark} 之前")
        return False
    
    # 构建要插入的内容：每个库对应一行 import_shared_library(lib_name)
    insert_lines = []
    for lib_name in lib_list:
        # 按 CMake 规范添加缩进（4个空格，可根据你的文件格式调整）
        insert_lines.append(f"    import_shared_library({lib_name})\n")
    
    # 替换两个标记之间的内容（保留标记行，中间插入新内容）
    # 新内容 = 标记前的行 + 开始标记行 + 插入内容 + 结束标记行 + 标记后的行
    new_lines = (
        lines[:start_idx + 1]  # 开始标记及之前的行
        + insert_lines         # 插入的 import_shared_library 语句
        + lines[end_idx:]      # 结束标记及之后的行
    )
    
    # 写入修改后的内容（覆盖原文件，建议先备份）
    try:
        # 可选：创建文件备份（如需要，取消下面两行注释）
        # backup_file = cmake_file.with_suffix(".cmake.bak")
        # with open(backup_file, "w", encoding="utf-8") as f:
        #     f.write("".join(lines))
        # print(f"已创建备份文件：{backup_file}")
        
        with open(cmake_file, "w", encoding="utf-8") as f:
            f.write("".join(new_lines))
        
        print(f"成功修改文件：{cmake_file}")
        print(f"已插入 {len(insert_lines)} 条 import_shared_library 语句：")
        for line in insert_lines:
            print(f"  {line.strip()}")
        return True
    except Exception as e:
        print(f"错误：写入文件 {cmake_file} 失败 - {str(e)}")
        return False

success = insert_import_shared_library(current_script_dir_out_Cmake, lib_out)
if not success:
    print("插入操作失败，请检查上述错误信息！")


def process_directory_level(current_dir):
    """
    处理单个目录层级：
    1. 检查当前目录下是否存在名为 "out" 的子目录
    2. 若存在，删除当前目录下除 "out" 外的所有其他子目录
    3. 递归处理当前目录下的所有子目录（包括 out 目录）
    """
    try:
        # 列出当前目录下的所有子项（文件+目录）
        entries = list(current_dir.iterdir())
        
        # 筛选出当前目录下的子目录（排除文件）
        subdirs = [entry for entry in entries if entry.is_dir()]
        if not subdirs:
            # 无任何子目录，直接返回
            return
        
        # 检查当前层级是否存在 "out" 目录
        has_out_dir = any(subdir.name == "out" for subdir in subdirs)
        
        if has_out_dir:
            print(f"\n✅ 在目录 {current_dir} 中发现 'out' 目录，开始删除其他子目录...")
            # 遍历当前层级的所有子目录，删除非 "out" 的目录
            for subdir in subdirs:
                if subdir.name != "out":
                    try:
                        shutil.rmtree(subdir)
                        print(f"  已删除目录：{subdir}")
                    except Exception as e:
                        print(f"  ⚠️  删除目录 {subdir} 失败：{str(e)}")
        else:
            # 无 out 目录，仅输出日志（可选关闭）
            print(f"ℹ️  目录 {current_dir} 中未发现 'out' 目录，跳过删除")
        
        # 递归处理当前目录下的所有子目录（包括 out 目录，确保所有层级都被检查）
        for subdir in subdirs:
            if subdir.name == "out" or not has_out_dir:
                process_directory_level(subdir)
    
    except PermissionError:
        print(f"❌ 权限不足，无法访问目录 {current_dir}")
    except Exception as e:
        print(f"❌ 处理目录 {current_dir} 时出错：{str(e)}")

def recursive_process_target_dirs(target_dirs):
    """批量处理多个目标目录"""
    for target_dir in target_dirs:
        dir_path = Path(target_dir)
        print("="*80)
        print(f"开始处理目标目录：{dir_path}")
        print("="*80)
        
        # 检查目标目录是否存在
        if not dir_path.exists() or not dir_path.is_dir():
            print(f"❌ 目录 {dir_path} 不存在或不是目录，跳过处理")
            continue
        
        # 开始递归处理
        process_directory_level(dir_path)
    
    print("\n" + "="*80)
    print("所有目标目录处理完成！")


target_dirs = [
    current_script_dir_out_src,  # 第一个目标目录
    current_script_dir_out_config  # 第二个目标目录
]

# 开始处理
recursive_process_target_dirs(target_dirs)



def process_header_files(root_dir):
    """
    递归遍历目录及子目录，处理所有 .h/.hpp 文件
    删除 // script auto start -> IN_USE 和 // script auto end -> IN_USE 之间的内容（保留标记行）
    """
    # 定义要匹配的文件后缀
    target_extensions = ('.h', '.hpp')
    # 定义区间起始和结束标记（精确匹配）
    start_marker = '// script auto start -> IN_USE'
    end_marker = '// script auto end -> IN_USE'
    
    # 递归遍历目录
    for dirpath, dirnames, filenames in os.walk(root_dir):
        for filename in filenames:
            # 筛选目标后缀的文件
            if filename.endswith(target_extensions):
                file_path = os.path.join(dirpath, filename)
                print(f"正在处理文件: {file_path}")
                
                try:
                    # 读取文件内容（使用 utf-8 编码，遇到编码错误忽略）
                    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                        lines = f.readlines()
                    
                    # 标记是否处于要删除的区间内
                    in_delete_range = False
                    new_lines = []
                    
                    for line in lines:
                        # 去除行首尾空白（避免因空格导致匹配失败）
                        stripped_line = line.strip()
                        
                        # 遇到起始标记：添加标记行，开启删除模式
                        if stripped_line == start_marker:
                            new_lines.append(line)
                            in_delete_range = True
                        # 遇到结束标记：关闭删除模式，添加标记行
                        elif stripped_line == end_marker:
                            in_delete_range = False
                            new_lines.append(line)
                        # 不在删除区间内：保留该行
                        elif not in_delete_range:
                            new_lines.append(line)
                    
                    # 只有当内容发生变化时才写入文件（避免不必要的文件修改）
                    if new_lines != lines:
                        with open(file_path, 'w', encoding='utf-8', errors='ignore') as f:
                            f.writelines(new_lines)
                        print(f"✅ 文件处理完成: {file_path}")
                    else:
                        print(f"ℹ️ 文件无匹配内容，无需修改: {file_path}")
                
                except Exception as e:
                    print(f"❌ 处理文件 {file_path} 时出错: {str(e)}")

process_header_files(current_script_dir_out_inc)
     
