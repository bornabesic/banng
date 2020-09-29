
add_library(random STATIC random.cpp)
add_library(time STATIC time.cpp)

add_executable(main main.cpp)
target_link_libraries(main random time)

pybind11_add_module(banng array.cpp)