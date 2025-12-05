project(f.win32)
add_library(f.win32 STATIC)
configure_target(f.win32)
target_link_libraries(f.win32 PUBLIC f)