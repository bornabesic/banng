
add_library(random STATIC random.cpp)

add_executable(main main.cpp)
target_link_libraries(main random)