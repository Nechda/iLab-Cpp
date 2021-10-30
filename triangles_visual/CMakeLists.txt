cmake_minimum_required(VERSION 3.10)

project(iLab-triangles-visual)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(WIN32)
    message("==============WIN32 LIB FINDING==============")

    find_library(GLFW_LIB NAMES glfw3 PATHS "${GLFW_DIR}/*")
    find_path(GLFW_INC NAMES GLFW PATHS "${GLFW_DIR}/*" NO_DEFAULT_PATH)
    message("-- Detecting glfw")
    message("-- Detecting glfw - library: ${GLFW_LIB}")
    message("-- Detecting glfw - include: ${GLFW_INC}")
    include_directories("${GLFW_INC}")
    if (${GLFW_LIB} STREQUAL "GLFW_LIB-NOTFOUND" OR ${GLFW_INC} STREQUAL "GLFW_INC-NOTFOUND")
        message(
            FATAL_ERROR
            "Can`t find glfw library, check your are writed right path\n"
            "use -DGLFW_DIR=<path_to_GLFW_dir>"
        )
    endif()

    find_path(GLM_INC NAMES glm PATHS "${GLM_DIR}/*" NO_DEFAULT_PATH)
    message("-- Detecting glm")
    message("-- Detecting glm - library: not required")
    message("-- Detecting glm - include: ${GLM_INC}")
    include_directories("${GLM_INC}")
    if (${GLM_INC} STREQUAL "GLM_INC-NOTFOUND")
        message(
            FATAL_ERROR
            "Can`t find glm library, check your are writed right path\n"
            "use -DGLM_DIR=<path_to_glm_dir>"
        )
    endif()

    find_library(VULKAN_LIB NAMES vulkan-1 PATHS "${VULKAN_DIR}\\Lib32")
    find_path(VULKAN_INC NAMES vulkan PATHS "${VULKAN_DIR}/*" NO_DEFAULT_PATH)
    message("-- Detecting vulkan")
    message("-- Detecting vulkan - library: ${VULKAN_LIB}")
    message("-- Detecting vulkan - include: ${VULKAN_INC}")
    include_directories("${VULKAN_INC}")
    if (${VULKAN_LIB} STREQUAL "VULKAN_LIB-NOTFOUND" OR ${VULKAN_INC} STREQUAL "VULKAN_INC-NOTFOUND")
        message(
            FATAL_ERROR
            "Can`t find vulkan library, check your are writed right path\n"
            "use -DVULKAN_DIR=<path_to_vulkan_dir>"
        )
    endif()

    message("=============================================")

    set(CMAKE_CXX_FLAGS "/w /EHsc /sdl-")
else(WIN32)
    set(GLFW_LIB "-lglfw")
    set(VULKAN_LIB "-lvulkan")
	set(CMAKE_CXX_FLAGS "-w")
endif()

file(GLOB_RECURSE GLSL_FILES "Shaders/shader.*.glsl")
set(GLSL_VALIDATOR glslangValidator)

foreach(GLSL ${GLSL_FILES})
	get_filename_component(FILE_NAME ${GLSL} NAME)
	set(SPV "${FILE_NAME}.spv")
	add_custom_command(
		OUTPUT ${SPV}
		COMMAND ${GLSL_VALIDATOR} ${GLSL} -V -o ${SPV}
		DEPENDS ${GLSL}
	)
	list(APPEND SPV_BINARY_FILES ${SPV})
endforeach()

add_custom_target(
	Shaders
	DEPENDS ${SPV_BINARY_FILES}
)

add_executable(
    triangles
    main.cpp
    Octree/triangle.cpp
    Lowering/Buffer.cpp
    Lowering/Descriptors.cpp
    Lowering/Device.cpp
    Lowering/Mesh.cpp
    Lowering/Pipeline.cpp
    Lowering/SwapChain.cpp
    Lowering/Window.cpp
)
add_dependencies(triangles Shaders)

target_link_libraries(triangles ${GLFW_LIB} ${VULKAN_LIB})