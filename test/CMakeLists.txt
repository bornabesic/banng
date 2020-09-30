
include(CTest)

add_executable(test_l2_distance test_l2_distance.cpp)
target_link_libraries(test_l2_distance gtest gtest_main)
add_test(NAME test_l2_distance COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_l2_distance)
