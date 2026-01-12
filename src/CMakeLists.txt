function(append_config_function FILE_PATH)
    # 获取文件所在目录
    get_filename_component(DIR_PATH "${FILE_PATH}" DIRECTORY)
    
    # 检查文件是否存在
    if(NOT EXISTS "${FILE_PATH}")
        message(STATUS "文件不存在，正在创建: ${FILE_PATH}")
        set(SOURCE_CONFIG "${PROJECT_ROOT}/config/cmake_demo/CMakeLists.txt")
        # 检查源配置文件是否存在
        if(EXISTS "${SOURCE_CONFIG}")
            message(STATUS "复制 .config 文件: ${SOURCE_CONFIG} -> ${FILE_PATH}")
            file(COPY "${SOURCE_CONFIG}" DESTINATION "${DIR_PATH}")
        else()
            message(WARNING "源配置文件不存在: ${SOURCE_CONFIG}，创建空文件")
            file(WRITE "${FILE_PATH}" "")  # 创建空文件
        endif()

        
        # 检查目录下是否有 .config 文件
        set(CONFIG_FILE "${DIR_PATH}/.config")
        if(NOT EXISTS "${CONFIG_FILE}")
            set(SOURCE_CONFIG "${PROJECT_ROOT}/config/cmake_demo/.config")
            
            # 检查源配置文件是否存在
            if(EXISTS "${SOURCE_CONFIG}")
                message(STATUS "复制 .config 文件: ${SOURCE_CONFIG} -> ${CONFIG_FILE}")
                file(COPY "${SOURCE_CONFIG}" DESTINATION "${DIR_PATH}")
            else()
                message(WARNING "源配置文件不存在: ${SOURCE_CONFIG}，创建空文件")
                file(WRITE "${CONFIG_FILE}" "")  # 源文件不存在时创建空文件
            endif()
        endif()
    endif()

    # 读取文件内容
    file(READ "${FILE_PATH}" CONTENT)

    # 检查是否包含目标字符串
    string(FIND "${CONTENT}" "Config_Mod_Dir_Cmake()" FOUND_POS)
    if(FOUND_POS EQUAL -1)
        message(STATUS "未找到 Config_Mod_Dir_Cmake()，正在追加...")
        
        # 直接添加换行符和函数调用
        file(APPEND "${FILE_PATH}" "\n\nConfig_Mod_Dir_Cmake()\n")
        message(STATUS "已成功追加到文件: ${FILE_PATH}")
    endif()
endfunction()

function(Config_Mod_Dir_Cmake)
    Get_All_Dir(MODULE_LIST)
    foreach(MODULE ${MODULE_LIST})
        # 进入模块目录
        set(CURRENT_MODULE_DIR ${MODULE})
        
        # 获取模块变量
        get_current_module_or_project_var("MODULE" CURRENT_VAR)
        message(STATUS "Checking ${MODULE} module: ${CURRENT_VAR}")

        append_config_function("${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/CMakeLists.txt")
        
        # 检查变量是否存在
        string(TOUPPER "${MODULE}" MODULE_UPPER)
        set(FULL_MODULE_VAR ${CURRENT_VAR}-${MODULE_UPPER})
        if(NOT DEFINED ${FULL_MODULE_VAR})
            message(WARNING "Variable ${FULL_MODULE_VAR} not defined, defaulting to OFF")
            set(${FULL_MODULE_VAR} OFF)
        endif()

        # 根据变量值决定是否编译
        if(${CURRENT_VAR}-${MODULE_UPPER})
            add_subdirectory(${MODULE})
        else()
            message(STATUS "Skipping ${MODULE} module (disabled in config)")
        endif()
    endforeach()
endfunction()

# 统一处理所有基础模块
Get_All_Dir(MODULE_LIST)
foreach(MODULE ${MODULE_LIST})
    # 进入模块目录
    set(CURRENT_MODULE_DIR ${MODULE})
    
    # 获取模块变量
    get_current_module_or_project_var("MODULE" CURRENT_VAR)
    # message(STATUS "Checking ${MODULE} module: ${CURRENT_VAR}")

    append_config_function("${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/CMakeLists.txt")

    # 检查变量是否存在
    string(TOUPPER "${MODULE}" MODULE_UPPER)
    set(FULL_MODULE_VAR "BUILD_MODULE_${MODULE_UPPER}")
    if(NOT DEFINED ${FULL_MODULE_VAR})
        message(WARNING "Variable ${FULL_MODULE_VAR} not defined, defaulting to OFF")
        set(${FULL_MODULE_VAR} OFF)
    endif()
    
    # 根据变量值决定是否编译
    if(${CURRENT_VAR}${MODULE_UPPER})
        add_subdirectory(${MODULE})
    else()
        message(STATUS "Skipping ${MODULE} module (disabled in config)")
    endif()
endforeach()

