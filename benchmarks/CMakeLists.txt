add_executable(ozo_benchmark ozo_benchmark.cpp)
target_link_libraries(ozo_benchmark ozo)

find_package(Boost COMPONENTS coroutine context system thread atomic program_options REQUIRED)

include(ExternalProject)
ExternalProject_Add(
    NlohmannJson
    GIT_REPOSITORY "https://github.com/nlohmann/json.git"
    GIT_TAG v3.7.3
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR} -DBUILD_TESTING=OFF
    UPDATE_COMMAND ""
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR}/include)

# enable a bunch of warnings and make them errors
target_compile_options(ozo_benchmark PRIVATE -Wall -Wextra -Wsign-compare -pedantic -Werror)

# ignore specific error for clang
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(ozo_benchmark PRIVATE -Wno-ignored-optimization-argument)
endif()

add_executable(ozo_benchmark_performance performance.cpp)
add_dependencies(ozo_benchmark_performance NlohmannJson)
target_link_libraries(ozo_benchmark_performance ozo)
target_link_libraries(ozo_benchmark_performance Boost::program_options)

# enable a bunch of warnings and make them errors
target_compile_options(ozo_benchmark_performance PRIVATE -Wall -Wextra -Wsign-compare -pedantic -Werror)

# ignore specific error for clang
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(ozo_benchmark_performance PRIVATE -Wno-ignored-optimization-argument)
endif()
