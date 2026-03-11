# --------------------------
# 编译 X_hand_control_demo 可执行文件
# --------------------------
set(EXE_NAME X_hand_control_demo)
NAME_TO_VAR("PROJECT" "-${EXE_NAME}" OUT)
if(${OUT})
    # 添加可执行文件+源文件
    add_executable(${EXE_NAME} 
        ./X_Hand_Brushed_Out.cpp
    )
    
    target_include_directories(${EXE_NAME} PUBLIC
        ${YAML_CPP_INCLUDE_DIR}
        ${SYS_INC_DIR}
        ${INC_SUBDIRS} 
    )

    add_dependencies(${EXE_NAME}
        HARDWARE_TOP
        Switch_Board
        Auto_Set_Id
        X_Hand_Protocol
    )

    config_debug_mode(${EXE_NAME})

    target_link_libraries(${EXE_NAME} PRIVATE
        HARDWARE_TOP
        Switch_Board
        Auto_Set_Id
        X_Hand_Protocol

        pthread
        -Wl,--no-as-needed 
        ${YAML_CPP_LIB}      
        spdlog_global    
        -Wl,--as-needed      
        )
endif()

Config_Mod_Dir_Cmake()