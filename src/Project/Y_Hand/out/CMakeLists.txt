# --------------------------
# 编译 Y_Hand_Demo 可执行文件
# --------------------------
set(EXE_NAME Y_Hand_Demo)
NAME_TO_VAR("PROJECT" "-${EXE_NAME}" OUT)
if(${OUT})
    # 添加可执行文件+源文件
    add_executable(${EXE_NAME} 
        ./Y_Hand_Out.cpp
    )

    target_include_directories(${EXE_NAME} PUBLIC
        ${YAML_CPP_INCLUDE_DIR}
        ${SYS_INC_DIR}
        ${INC_DIR}/Motor
        ${INC_DIR}/Switch_Board
        ${INC_DIR}/Auto_Set_Id
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

# --------------------------
# 编译 hand_pretighten 可执行文件
# --------------------------
set(EXE_NAME hand_pretighten)
NAME_TO_VAR("PROJECT" "-${EXE_NAME}" OUT)
if(${OUT})
    # 添加可执行文件+源文件
    add_executable(${EXE_NAME} 
        ./hand_pretighten.cpp
    )

    target_include_directories(${EXE_NAME} PUBLIC
        ${YAML_CPP_INCLUDE_DIR}
        ${SYS_INC_DIR}
        ${INC_DIR}/Motor
        ${INC_DIR}/Switch_Board
        ${INC_DIR}/Auto_Set_Id
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


