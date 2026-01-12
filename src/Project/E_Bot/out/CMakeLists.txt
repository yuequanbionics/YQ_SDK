# --------------------------
# 编译 E_hand_demo 可执行文件
# --------------------------
set(EXE_NAME E_hand_demo)
NAME_TO_VAR("PROJECT" "-${EXE_NAME}" OUT)
if(${OUT})
    # 添加可执行文件+源文件
    add_executable(${EXE_NAME}
            ./E_Bot_Out.cpp
    )

    target_include_directories(${EXE_NAME} PUBLIC
            ${YAML_CPP_INCLUDE_DIR}
            ${SYS_INC_DIR}
            ${INC_DIR}/Motor
            ${INC_DIR}/Switch_Board
            ${INC_DIR}/Custom_Device/Servo_Feetech
    )

    add_dependencies(${EXE_NAME}
            HARDWARE_TOP
            Switch_Board
    )

    config_debug_mode(${EXE_NAME})

    target_link_libraries(${EXE_NAME} PRIVATE
            HARDWARE_TOP
            Switch_Board
            Servo_Feetech_SCS0009

            pthread
            -Wl,--no-as-needed
            ${YAML_CPP_LIB}
            spdlog_global
            -Wl,--as-needed
    )
endif()

Config_Mod_Dir_Cmake()


