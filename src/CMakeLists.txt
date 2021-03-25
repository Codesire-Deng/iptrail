cmake_minimum_required(VERSION 3.10.0)

# 设置exe输出目录为./build
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

# main.cpp -> main.exe （当然还需要链接各种库）
add_executable(iptrail main.cpp email.cpp mybio.cpp token.cpp)
# add_executable(ref ref.cpp)
# add_executable(try try.cpp)
# add_executable(mybio mybio.cpp)
# add_executable(timesvr timesvr.cpp)
# add_executable(timecli timecli.cpp)

# 链接自定义库
# target_link_libraries(main PRIVATE common object)

# 链接第三方库
target_link_libraries(iptrail PRIVATE libcrypto libboost_json)

# -------------------------------------------------
# 到这里 Exe 已经生成完了。 接下来将运行时需要的文件复制一下
# -------------------------------------------------

# 拷贝动态库文件到build/Debug
# add_custom_command(TARGET main POST_BUILD       # Adds a post-build event to main
#     COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_if_different..."
#     "${PROJECT_SOURCE_DIR}/lib"                 # <--this is in-file
#     $<TARGET_FILE_DIR:main>                     # <--this is out-file path
# )                

# 拷贝shaders到build/Debug/shaders
# add_custom_command(TARGET main POST_BUILD  
#     COMMAND ${CMAKE_COMMAND} -E copy_directory 
#     "${PROJECT_SOURCE_DIR}/shaders"     
#     $<TARGET_FILE_DIR:main>/shaders
# )    

# 拷贝assets到build/Debug/assets
# add_custom_command(TARGET main POST_BUILD   
#     COMMAND ${CMAKE_COMMAND} -E copy_directory  
#     "${PROJECT_SOURCE_DIR}/assets"      
#     $<TARGET_FILE_DIR:main>/assets
# )    

# 拷贝capture到build/Debug/assets
# add_custom_command(TARGET main POST_BUILD   
#     COMMAND ${CMAKE_COMMAND} -E copy_directory  
#     "${PROJECT_SOURCE_DIR}/capture"      
#     $<TARGET_FILE_DIR:main>/capture
# )