cmake_minimum_required(VERSION 3.21)

project(plague LANGUAGES C)

add_subdirectory(extern/glfw)

set(VOLK_HEADERS_ONLY ON)
add_subdirectory(extern/volk)

add_executable(plague src/main.c)

target_link_libraries(plague 
	PRIVATE glfw
	PRIVATE volk_headers
)

target_compile_definitions(plague
	PRIVATE $<$<CONFIG:Debug>:PLAGUE_DEBUG>
	PRIVATE $<$<PLATFORM_ID:Windows>:VK_USE_PLATFORM_WIN32_KHR>
	PRIVATE $<$<PLATFORM_ID:Linux>:VK_USE_PLATFORM_XLIB_KHR>
)
