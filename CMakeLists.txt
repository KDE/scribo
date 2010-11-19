project(scribo)

include_directories(${scribo_SOURCE_DIR}/lib)

add_subdirectory(lib)
add_subdirectory(shell)
add_subdirectory(plugins)
add_subdirectory(service)
