cmake_minimum_required(VERSION 3.10)

project(iLab-triangles-visual)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-w")


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
target_link_libraries(triangles "-lglfw -lvulkan")

add_dependencies(triangles Shaders)
