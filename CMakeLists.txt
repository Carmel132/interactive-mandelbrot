cmake_minimum_required(VERSION 3.28)
set(CMAKE_TOOLCHAIN_FILE "C:/Users/User/vcpkg/scripts/buildsystems/vcpkg.cmake" 
    CACHE STRING "")
project (main VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 CONFIG REQUIRED)

file(GLOB SOURCES 
	"src/*.cpp"
	"main.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE
	SDL3::SDL3
)  
target_include_directories(main
	PRIVATE
		${PROJECT_SOURCE_DIR}/rsrc
		${PROJECT_SOURCE_DIR}/src
		${PROJECT_SOURCE_DIR}
)


