# # --------------------------
# # 编译可执行文件
# # --------------------------
# set(EXE_NAME Your_Exe_Name)
# NAME_TO_VAR("PROJECT" "-${EXE_NAME}" OUT)
# if(${OUT})
#     # 添加可执行文件+源文件
#     add_executable(${EXE_NAME} 
#         # ./Test.cpp
#     )

#     target_include_directories(${EXE_NAME} PUBLIC
#         ${YAML_CPP_INCLUDE_DIR}
#         ${SYS_INC_DIR}
#         # ${INC_DIR}/Motor
#         # ${INC_DIR}/Switch_Board
#     )

#     add_dependencies(${EXE_NAME}
#         # spdlog_global
#         # HARDWARE_TOP
#         # Switch_Board
#         # Motor_Device
#     )

#     NAME_TO_VAR("PROJECT" "-${EXE_NAME}_DEBUG" DEBUG_OUT)
#     if(${DEBUG_OUT})
#         target_compile_definitions(${EXE_NAME} PRIVATE
#             DEBUG_MOD 
#         )
#     endif()

#     target_link_libraries(${EXE_NAME} PRIVATE
#         # HARDWARE_TOP
#         # Switch_Board
#         # Motor_Device

#         # pthread
#         # -Wl,--no-as-needed 
#         # ${YAML_CPP_LIB}      
#         # -Wl,--as-needed  
#         # spdlog_global    
#     )
# endif()

Config_Mod_Dir_Cmake()
