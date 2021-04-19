cmake_minimum_required(VERSION 3.12)
project(my_ozo_using_project)

find_package(ozo REQUIRED)
add_executable(my_app ../../examples/request_coroutine.cpp)
target_link_libraries(my_app PRIVATE yandex::ozo)
