# --------------------------
# 编译 W_Bot_Out 可执行文件
# --------------------------
set(EXE_NAME W_Bot_Out)
NAME_TO_VAR("PROJECT" "-${EXE_NAME}" OUT)
if(${OUT})
    # 添加可执行文件+源文件
    add_executable(${EXE_NAME} 
        ./W_Bot_Out.cpp
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
        Eyou_Custom_Motor_Device
        Motor_Device_TaiHu
        Led_Device
        Uart_Custom_Dome_Device
        Motor_Device_BenMo
        Battery_BMS_V2_Device
        IMU_Device_YuanJi
    )

    config_debug_mode(${EXE_NAME})

    target_link_libraries(${EXE_NAME} PRIVATE
        HARDWARE_TOP
        Switch_Board
        Auto_Set_Id
        Eyou_Custom_Motor_Device
        Motor_Device_TaiHu
        Led_Device
        Uart_Custom_Dome_Device
        Motor_Device_BenMo
        Battery_BMS_V2_Device
        IMU_Device_YuanJi

        pthread
        -Wl,--no-as-needed 
        ${YAML_CPP_LIB}      
        spdlog_global    
        -Wl,--as-needed      
        )
endif()

Config_Mod_Dir_Cmake()


