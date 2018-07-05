define_module(pylp LIBRARY LINKS solvers boost-python)
add_custom_target(rename_pylp_lib ALL COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/python/libpylp.so ${CMAKE_BINARY_DIR}/python/pylp.so)
add_dependencies(rename_pylp_lib pylp)
install(TARGETS pylp LIBRARY DESTINATION lib)
