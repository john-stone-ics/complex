find_package(pybind11 2.6.2 CONFIG REQUIRED)

pybind11_add_module(complexpy complexpy.cpp)

# we want the CMake target name to be "complexpy", but the Python module to be "complex"
set_target_properties(complexpy
  PROPERTIES
    OUTPUT_NAME complex
    LIBRARY_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:complex>
)

target_link_libraries(complexpy
  PUBLIC
    complex
)

target_compile_options(complexpy
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/MP>
)
