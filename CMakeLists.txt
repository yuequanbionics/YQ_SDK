cmake_minimum_required(VERSION 3.10)
project(RHS)   
# --------------------------
# 基础配置与项目结构定义
# --------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 来自 yaml-cpp 库的关于 std::iterator 已过时的警告 在编译时添加编译选项以忽略此警告
add_compile_options(-Wno-deprecated-declarations)

# 定义项目绝对路径结构（单一定义点）
get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
message(STATUS "Project root: ${PROJECT_ROOT}")

set(SRC_DIR ${PROJECT_ROOT}/src)
set(INC_DIR ${PROJECT_ROOT}/inc)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    set(LIB_DIR ${PROJECT_ROOT}/lib/x86_64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    set(LIB_DIR ${PROJECT_ROOT}/lib/arch64)
else()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(THIRD_PARTY_DIR ${PROJECT_ROOT}/third_party)

set(SYS_INC_DIR 
    ${INC_DIR} 
    ${INC_DIR}/HARDWARE_TOP 
    ${INC_DIR}/Z_SYS_SRC/CAN 
    ${INC_DIR}/Custom 
    ${INC_DIR}/Z_SYS_SRC/SYS 
    ${INC_DIR}/Z_SYS_SRC/UDP 
    ${PROJECT_ROOT}/third_party/lib/install-x86_64/include
    ${PROJECT_ROOT}/third_party/lib/install-x86_64/include/yaml-cpp
)

file(GLOB SYS_SRC_FILE 
    ${SRC_DIR}/*.cpp
    ${SRC_DIR}/Z_SYS_SRC/UDP/*.cpp 
    ${SRC_DIR}/Z_SYS_SRC/CAN/*.cpp 
    ${SRC_DIR}/Z_SYS_SRC/SYS/*.cpp 
)

# 输出项目结构信息
message(STATUS "===== PROJECT STRUCTURE =====")
message(STATUS "PROJECT_ROOT: ${PROJECT_ROOT}")
message(STATUS "SRC_DIR: ${SRC_DIR}")
message(STATUS "INC_DIR: ${INC_DIR}")
message(STATUS "LIB_DIR: ${LIB_DIR}")
message(STATUS "THIRD_PARTY_DIR: ${THIRD_PARTY_DIR}")
message(STATUS "===========================")

function(Get_All_Dir OUT)
    # 定义要排除的文件夹列表
    set(EXCLUDED_DIRS "Z_SYS_SRC")
    # 查找当前目录下的所有文件夹
    set(ALL_SUBDIRS "")
    file(GLOB ALL_ENTRIES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
    foreach(ENTRY ${ALL_ENTRIES})
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${ENTRY})
            # 检查是否在排除列表中
            list(FIND EXCLUDED_DIRS ${ENTRY} INDEX)
            if(INDEX EQUAL -1)
                list(APPEND ALL_SUBDIRS ${ENTRY})
            endif()
        endif()
    endforeach()

    set(${OUT} ${ALL_SUBDIRS} PARENT_SCOPE) 

    # # 打印所有文件夹名称
    # message("当前目录下的文件夹有:")
    # foreach(DIR ${ALL_SUBDIRS})
    #     message("  ${DIR}")
    # endforeach()
endfunction()


# --------------------------
# 路径转换函数定义
# --------------------------
function(convert_path_to_variable PATH_TYPE PATH OUTPUT_VAR)
    # 替换所有/为-
    string(REPLACE "/" "-" VAR_NAME ${PATH})
    # 移除重复的-（如果有）
    string(REGEX REPLACE "-+" "-" VAR_NAME ${VAR_NAME})
    # 移除开头和结尾的-（如果有）
    string(REGEX REPLACE "^-|-$" "" VAR_NAME ${VAR_NAME})
    # 转换为大写
    string(TOUPPER ${VAR_NAME} VAR_NAME)
    # 根据类型设置前缀
    if(PATH_TYPE STREQUAL "MODULE")
        set(${OUTPUT_VAR} "BUILD_MODULE_${VAR_NAME}" PARENT_SCOPE)
    else() # PROJECT
        set(${OUTPUT_VAR} "BUILD_PROJECT_${VAR_NAME}" PARENT_SCOPE)
    endif()
endfunction()

# 获取相对于项目根目录的相对路径
function(get_relative_path ABSOLUTE_PATH OUTPUT_VAR)
    # 计算相对路径
    file(RELATIVE_PATH REL_PATH ${PROJECT_ROOT} ${ABSOLUTE_PATH})
    # 移除开头的../
    string(REGEX REPLACE "^\\.\\.?/" "" CLEAN_PATH ${REL_PATH})
    set(${OUTPUT_VAR} ${CLEAN_PATH} PARENT_SCOPE)
endfunction()

# 自动获取当前CMakeLists.txt的模块或项目变量名
# 参数：
#   TYPE - 指定"MODULE"或"PROJECT"
#   OUTPUT_VAR - 输出变量名
function(get_current_module_or_project_var TYPE OUTPUT_VAR)
    # 验证输入类型
    if(NOT TYPE STREQUAL "MODULE" AND NOT TYPE STREQUAL "PROJECT")
        message(FATAL_ERROR "Invalid type '${TYPE}'. Must be 'MODULE' or 'PROJECT'")
    endif()

    # 获取当前CMakeLists.txt的绝对路径
    get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_LIST_DIR} ABSOLUTE)
    
    # 转换为相对于项目根目录的路径
    get_relative_path(${CURRENT_DIR} REL_PATH)
    # message(STATUS "Relative path from project root: ${REL_PATH}")
    
    # 处理特殊情况：如果是src目录本身
    if(REL_PATH STREQUAL "src")
        if(TYPE STREQUAL "MODULE")
            set(FULL_PATH "")
            # message(STATUS "Root module detected")
        else()
            message(FATAL_ERROR "Cannot use PROJECT type for src directory")
        endif()
    else()
        # 提取模块路径部分（去掉src/前缀）
        string(REGEX REPLACE "^src/" "" MODULE_PATH ${REL_PATH})
        # message(STATUS "Module path without src/: ${MODULE_PATH}")
        
        if(TYPE STREQUAL "MODULE")
            # 对于模块，直接使用模块路径
            set(FULL_PATH "${MODULE_PATH}")
            # message(STATUS "Module path: ${FULL_PATH}")
        else() # PROJECT
            # 对于项目，路径即为模块路径（不再添加额外目录名）
            set(FULL_PATH "${MODULE_PATH}")
            # message(STATUS "Project path: ${FULL_PATH}")
        endif()
    endif()
    
    # 转换为变量名
    if(FULL_PATH STREQUAL "")
        # 处理根模块的特殊情况
        set(RESULT_VAR "BUILD_MODULE_")
    else()
        convert_path_to_variable(${TYPE} ${FULL_PATH} RESULT_VAR)
    endif()
    
    set(${OUTPUT_VAR} ${RESULT_VAR} PARENT_SCOPE)
    # message(STATUS "Generated ${TYPE} variable: ${RESULT_VAR}")
endfunction()

function(NAME_TO_VAR TYPE NAME OUT)
    string(TOUPPER "${NAME}" NAME_UPPER)
    get_current_module_or_project_var(${TYPE} BASE_VAR)
    set(${OUT} "${BASE_VAR}${NAME_UPPER}" PARENT_SCOPE)
endfunction()

# --------------------------
# 解析 config.txt 配置
# --------------------------
set(CONFIG_FILE "${PROJECT_ROOT}/config.txt")
unset(MODULE_ENABLED_)  # 清除缓存的模块状态
unset(PROJECT_ENABLED_) # 清除缓存的子项目状态
set(CURRENT_MODULE "")     # 当前解析的模块名
set(ALL_MODULES "")        # 收集所有模块路径
set(ALL_PROJECTS "")       # 收集所有项目路径

if(EXISTS ${CONFIG_FILE})
    file(STRINGS ${CONFIG_FILE} CONFIG_LINES)
    
    foreach(LINE ${CONFIG_LINES})
        string(STRIP ${LINE} LINE)
        
        # 跳过空行和注释
        if(LINE STREQUAL "" OR LINE MATCHES "^#")
            continue()
        endif()

        # 1. 匹配模块路径（格式：[模块路径]）
        if(LINE MATCHES "^\\[(.*)\\]$")
            string(REGEX REPLACE "^\\[(.*)\\]$" "\\1" CURRENT_MODULE ${LINE})
            
            # 生成模块变量名
            convert_path_to_variable("MODULE" ${CURRENT_MODULE} MODULE_VAR)
            
            # 初始化模块状态（默认OFF，后续被ENABLE=1/0覆盖）
            if(NOT DEFINED ${MODULE_VAR})
                set(${MODULE_VAR} OFF CACHE INTERNAL "")
            endif()
            
            # 记录所有模块路径
            list(APPEND ALL_MODULES ${CURRENT_MODULE})
        # 2. 匹配模块的ENABLE状态（格式：ENABLE=1/0）
        elseif(LINE MATCHES "^ENABLE=(1|0)$" AND NOT CURRENT_MODULE STREQUAL "")
            string(REGEX REPLACE "^ENABLE=(1|0)$" "\\1" ENABLE_VALUE ${LINE})
            convert_path_to_variable("MODULE" ${CURRENT_MODULE} MODULE_VAR)
            
            if(ENABLE_VALUE STREQUAL "1")
                set(${MODULE_VAR} ON CACHE INTERNAL "")
            else()
                set(${MODULE_VAR} OFF CACHE INTERNAL "")
            endif()
        # 3. 匹配项目的ENABLE状态（格式：ENABLE_项目名=ON/OFF）
        elseif(LINE MATCHES "^ENABLE_(.*)=(ON|OFF|1|0)$" AND NOT CURRENT_MODULE STREQUAL "")
            string(REGEX REPLACE "^ENABLE_(.*)=(ON|OFF|1|0)$" "\\1" PROJECT_NAME ${LINE})
            string(REGEX REPLACE "^ENABLE_(.*)=(ON|OFF|1|0)$" "\\2" PROJECT_VALUE ${LINE})
            
            # 生成带路径的项目全名
            set(FULL_PROJECT_NAME "${CURRENT_MODULE}/${PROJECT_NAME}")
            convert_path_to_variable("PROJECT" ${FULL_PROJECT_NAME} PROJECT_VAR)
            
            # 转换为布尔值
            if(PROJECT_VALUE STREQUAL "ON" OR PROJECT_VALUE STREQUAL "1")
                set(${PROJECT_VAR} ON CACHE INTERNAL "")
            else()
                set(${PROJECT_VAR} OFF CACHE INTERNAL "")
            endif()
            
            list(APPEND ALL_PROJECTS ${FULL_PROJECT_NAME})
        endif()
    endforeach()
endif()

# 添加文件依赖，确保config.txt变更后重新解析
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CONFIG_FILE})

# --------------------------
# 打印配置信息
# --------------------------
message(STATUS "===== CONFIGURATION SUMMARY =====")
message(STATUS "Found modules:")
foreach(MODULE_PATH ${ALL_MODULES})
    convert_path_to_variable("MODULE" ${MODULE_PATH} MODULE_VAR)
    message(STATUS "  [${MODULE_PATH}] -> ${MODULE_VAR} = ${${MODULE_VAR}}")
endforeach()

message(STATUS "Found projects:")
foreach(PROJECT_PATH ${ALL_PROJECTS})
    convert_path_to_variable("PROJECT" ${PROJECT_PATH} PROJECT_VAR)
    message(STATUS "  ${PROJECT_PATH} -> ${PROJECT_VAR} = ${${PROJECT_VAR}}")
endforeach()
message(STATUS "================================")

# --------------------------
# 第三方库公共配置
# --------------------------
# 统一架构判断，获取安装目录后缀
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    set(ARCH_SUFFIX "x86_64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    set(ARCH_SUFFIX "arm64")
else()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

# 公共安装路径
set(THIRD_PARTY_INSTALL_DIR "${PROJECT_ROOT}/third_party/lib/install-${ARCH_SUFFIX}")

# --------------------------
# YAML-CPP 配置
# --------------------------
set(YAML_CPP_INCLUDE_DIR "${THIRD_PARTY_INSTALL_DIR}/include/yaml-cpp")
set(YAML_CPP_LIB_DIR "${THIRD_PARTY_INSTALL_DIR}/lib")

# 查找库文件
find_library(YAML_CPP_LIB
    NAMES yaml-cpp libyaml-cpp.so
    PATHS ${YAML_CPP_LIB_DIR}
    NO_DEFAULT_PATH
)

# --------------------------
# spdlog_global 配置
# --------------------------
add_library(spdlog_global SHARED IMPORTED)
set_target_properties(spdlog_global PROPERTIES
    IMPORTED_LOCATION "${THIRD_PARTY_INSTALL_DIR}/lib/libspdlog.so"
    INTERFACE_INCLUDE_DIRECTORIES "${THIRD_PARTY_INSTALL_DIR}/include/spdlog"
)
add_compile_definitions(SPDLOG_COMPILED_LIB)

# 封装调试模式配置函数：为指定目标设置 Debug 编译定义、编译选项和链接选项
# 参数：EXE_NAME - 目标可执行文件名称（必须已通过 add_executable 定义）
function(config_debug_mode EXE_NAME)
    # 校验参数：确保传入了目标名称
    if(NOT EXE_NAME)
        message(FATAL_ERROR "config_debug_mode 函数必须传入 EXE_NAME 参数（目标可执行文件名称）")
    endif()

    # 校验目标是否存在：避免为未定义的目标配置
    if(NOT TARGET ${EXE_NAME})
        message(FATAL_ERROR "目标 ${EXE_NAME} 未定义！请先通过 add_executable 定义目标")
    endif()

    # 构造 Debug 模式标识变量（保持原有的 NAME_TO_VAR 逻辑）
    NAME_TO_VAR("PROJECT" "-${EXE_NAME}_DEBUG" DEBUG_OUT)

    # 如果启用了该目标的 Debug 模式，则应用配置
    if(${DEBUG_OUT})
        message(STATUS "为目标 ${EXE_NAME} 启用 Debug 模式配置")

        # 添加 Debug 编译定义
        target_compile_definitions(${EXE_NAME} PRIVATE
            DEBUG_MOD
        )

        # 设置 Debug 编译选项（-g 调试信息、-O0 无优化、-Wall 开启所有警告）
        target_compile_options(${EXE_NAME} PRIVATE
            -g
            -O0
            -Wall
        )

        # 设置 Debug 链接选项（-rdynamic 导出符号，便于调试时查看符号信息）
        target_link_options(${EXE_NAME} PRIVATE
            -rdynamic
        )
    endif()
endfunction()

# 封装导入共享库的函数
# 参数：
#   TARGET_NAME: 导入库的目标名（如 HARDWARE_TOP）
function(import_shared_library TARGET_NAME)
    # 检查参数是否为空
    if(NOT TARGET_NAME)
        message(FATAL_ERROR "导入共享库失败：必须指定目标名（第一个参数）")
    endif()

    # 检查 LIB_DIR 变量是否已定义
    if(NOT DEFINED LIB_DIR)
        message(FATAL_ERROR "导入共享库失败：未定义 LIB_DIR 变量（请先设置库文件所在目录）")
    endif()

    # 检查库文件是否存在
    set(LIB_FILE "${LIB_DIR}/lib${TARGET_NAME}.so")
    if(NOT EXISTS ${LIB_FILE})
        message(FATAL_ERROR "导入共享库失败：库文件不存在 -> ${LIB_FILE}")
    endif()

    # 创建导入共享库目标
    add_library(${TARGET_NAME} SHARED IMPORTED)
    
    # 设置目标属性（导入路径）
    set_target_properties(${TARGET_NAME} PROPERTIES
        IMPORTED_LOCATION ${LIB_FILE}
    )

    # 可选：打印成功信息（便于调试）
    message(STATUS "成功导入共享库：${TARGET_NAME} -> ${LIB_FILE}")
endfunction()

# script auto start -> lib_add
    import_shared_library(Switch_Board)
    import_shared_library(Servo_Feetech_SCS0009)
    import_shared_library(Eyou_Custom_Motor_Device)
    import_shared_library(Motor_Interpolation)
    import_shared_library(Custom_Servo_Feetech_SCS0009)
    import_shared_library(HARDWARE_TOP)
    import_shared_library(IMU_Device_YuanJi)
    import_shared_library(Motor_Device)
    import_shared_library(Motor_Device_BenMo)
    import_shared_library(Uart_Custom_Dome_Device)
    import_shared_library(Motor_Device_TaiHu)
    import_shared_library(Custom_Motor_Device_DaMiao)
    import_shared_library(Pressure_Sensor_Custom_Device)
    import_shared_library(Auto_Set_Id)
    import_shared_library(Led_Device)
# script auto end -> lib_add

# 设置整个项目的编译输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_ROOT}/bin)

# 编译子项目
add_subdirectory(src)


