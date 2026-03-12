# --------------------------
# 编译可执行文件
# --------------------------
set(EXE_NAME IO_Board)
NAME_TO_VAR("PROJECT" "-${EXE_NAME}" OUT)
if(${OUT})
    # 添加可执行文件+源文件
    add_executable(${EXE_NAME}
            out/IO_Board_Out.cpp
    )

    target_include_directories(${EXE_NAME} PUBLIC
        ${YAML_CPP_INCLUDE_DIR}
        ${SYS_INC_DIR}
        ${INC_SUBDIRS} 
    )

    add_dependencies(${EXE_NAME}
        spdlog_global
        HARDWARE_TOP
        Switch_Board
    )

    config_debug_mode(${EXE_NAME})

    target_link_libraries(${EXE_NAME} PRIVATE
        HARDWARE_TOP
        Switch_Board

        pthread
        -Wl,--no-as-needed 
        ${YAML_CPP_LIB}
        spdlog_global
        -Wl,--as-needed
    )
endif()

Config_Mod_Dir_Cmake()
