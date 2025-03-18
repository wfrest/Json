find_program(CMAKE_MEMORYCHECK_COMMAND valgrind)
set(memcheck_command ${CMAKE_MEMORYCHECK_COMMAND} ${CMAKE_MEMORYCHECK_COMMAND_OPTIONS} --error-exitcode=1 --leak-check=full)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

enable_testing()

# Test utility header
set(TEST_UTILS_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/test_utils.h")

set(JSON_TEST_LIST
  simple
  destructor
  hotfix
  minimal
  iter
  json_arr
  json_compatibility
  json_error
  json_integration
  json_memory
  json_obj
  json_parse
  json_performance
  json_serialize
  json_type
)

# Function to add a test target
function(add_json_test test_name)
  add_executable(${test_name} EXCLUDE_FROM_ALL ${test_name}.cc)
  target_include_directories(${test_name} PUBLIC ${PROJECT_SOURCE_DIR})
  target_link_libraries(${test_name} Json pthread)
  add_test(NAME ${test_name} COMMAND ${test_name})
  add_dependencies(check ${test_name})
endfunction()

# Create individual test targets
foreach(test_name ${JSON_TEST_LIST})
  add_json_test(${test_name})
  
  # Create an explicit target that can be built with 'make test_xxx'
  add_custom_target(test_${test_name}
    DEPENDS ${test_name}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${test_name}
    COMMENT "Running ${test_name} tests"
  )
  
  # Add memory check test
  add_test(NAME ${test_name}-memory-check COMMAND ${memcheck_command} ${CMAKE_CURRENT_BINARY_DIR}/${test_name})
endforeach()

# Add a target to build all tests
add_custom_target(build_tests
  DEPENDS ${JSON_TEST_LIST}
  COMMENT "Building all tests"
)

# Add a target to run all tests
add_custom_target(run_tests
  DEPENDS build_tests
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
  COMMENT "Running all tests"
) 