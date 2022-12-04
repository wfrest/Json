find_program(CMAKE_MEMORYCHECK_COMMAND valgrind)
set(memcheck_command ${CMAKE_MEMORYCHECK_COMMAND} ${CMAKE_MEMORYCHECK_COMMAND_OPTIONS} --error-exitcode=1 --leak-check=full)

set(JSON_EXAMPLES
  01_create
  02_create_stl_like
  03_stringfy
  04_iter
)

foreach(src ${JSON_EXAMPLES})
  message(STATUS "example files found: ${src}.cc")
  add_executable(${src} EXCLUDE_FROM_ALL ${src}.cc)
  target_include_directories(${src} PUBLIC ${PROJECT_SOURCE_DIR})
  target_link_libraries(${src} Json)
endforeach()

foreach(src ${JSON_EXAMPLES})
  add_test(${src}-memory-check ${memcheck_command} ./${src})
endforeach()