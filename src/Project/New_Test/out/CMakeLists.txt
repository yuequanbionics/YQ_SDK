# --------------------------
# 编译Test可执行文件
# --------------------------
set(EXE_NAME Test)
NAME_TO_VAR("PROJECT" "-${EXE_NAME}" OUT)
if(${OUT})
    # 添加可执行文件+源文件
    add_executable(${EXE_NAME} 
        ./Test.cpp
    )    

    target_include_directories(${EXE_NAME} PUBLIC
        ${YAML_CPP_INCLUDE_DIR}
        ${SYS_INC_DIR}
        ${INC_SUBDIRS} 
    )

    add_dependencies(${EXE_NAME}
        HARDWARE_TOP
        Switch_Board
        Motor_Device
        Auto_Set_Id
    )

    config_debug_mode(${EXE_NAME})

    target_link_libraries(${EXE_NAME} PRIVATE
        HARDWARE_TOP
        Switch_Board
        Motor_Device
        Auto_Set_Id

        pthread
        -Wl,--no-as-needed 
        ${YAML_CPP_LIB}      
        spdlog_global    
        -Wl,--as-needed      
        )
endif()

Config_Mod_Dir_Cmake()


