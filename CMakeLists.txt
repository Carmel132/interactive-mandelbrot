cmake_minimum_required(VERSION 3.20)
set(CMAKE_TOOLCHAIN_FILE "C:/Users/User/vcpkg/scripts/buildsystems/vcpkg.cmake" 
    CACHE STRING "")
project (main LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
	main.cpp
)
find_package(SDL3 CONFIG REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME}
    PRIVATE SDL3::SDL3
)  
target_include_directories(main 
	PRIVATE
		${PROJECT_SOURCE_DIR}/rsrc
		${PROJECT_SOURCE_DIR}
)
