# Psycho_Core - mod-skeleton optional CMake hook
# -----------------------------------------------
# This file is OPTIONAL. If present, modules/CMakeLists.txt include()s it INLINE
# during configuration (after the `modules` target exists). Use it to add extra
# dependencies, include directories, compile definitions or find_package() calls
# that your module needs.
#
# The skeleton needs nothing extra, so everything here is commented out as a
# reference. Uncomment/adapt as needed.
#
# Example: link an external library into the static `modules` target:
#   find_package(SomeLib REQUIRED)
#   target_link_libraries(modules PUBLIC SomeLib::SomeLib)
#
# Example: add an include directory:
#   target_include_directories(modules PUBLIC "${CMAKE_CURRENT_LIST_DIR}/src/include")
#
# Example: add a compile definition only when this module is built:
#   target_compile_definitions(modules PUBLIC MOD_SKELETON_ENABLED)

message(STATUS "  mod-skeleton: optional .cmake loaded (no extra deps).")
