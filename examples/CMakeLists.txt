find_program(CCACHE_FOUND ccache)

if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

add_executable(ozo_request_coroutine request_coroutine.cpp)
target_link_libraries(ozo_request_coroutine ozo)

# enable a bunch of warnings and make them errors
target_compile_options(ozo_request_coroutine PRIVATE -Wall -Wextra -Wsign-compare -Werror)

# ignore specific errors for clang
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(ozo_request_coroutine PRIVATE -Wno-ignored-optimization-argument)
endif()

add_executable(ozo_transaction transaction.cpp)
target_link_libraries(ozo_transaction ozo)

# enable a bunch of warnings and make them errors
target_compile_options(ozo_transaction PRIVATE -Wall -Wextra -Wsign-compare -Werror)

# ignore specific errors for clang
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(ozo_transaction PRIVATE -Wno-ignored-optimization-argument)
endif()

add_executable(ozo_retry_request retry_request.cpp)
target_link_libraries(ozo_retry_request ozo)

# enable a bunch of warnings and make them errors
target_compile_options(ozo_retry_request PRIVATE -Wall -Wextra -Wsign-compare -Werror)

# ignore specific errors for clang
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(ozo_retry_request PRIVATE -Wno-ignored-optimization-argument)
endif()

add_executable(ozo_role_based_request role_based_request.cpp)
target_link_libraries(ozo_role_based_request ozo)

# enable a bunch of warnings and make them errors
target_compile_options(ozo_role_based_request PRIVATE -Wall -Wextra -Wsign-compare -Werror)

# ignore specific errors for clang
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(ozo_role_based_request PRIVATE -Wno-ignored-optimization-argument)
endif()

add_executable(ozo_connection_pool connection_pool.cpp)
target_link_libraries(ozo_connection_pool ozo)

# enable a bunch of warnings and make them errors
target_compile_options(ozo_connection_pool PRIVATE -Wall -Wextra -Wsign-compare -Werror)

# ignore specific errors for clang
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(ozo_connection_pool PRIVATE -Wno-ignored-optimization-argument)
endif()

add_executable(ozo_request_future request_future.cpp)
target_link_libraries(ozo_request_future ozo)

# enable a bunch of warnings and make them errors
target_compile_options(ozo_request_future PRIVATE -Wall -Wextra -Wsign-compare -Werror)

# ignore specific errors for clang
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(ozo_request_future PRIVATE -Wno-ignored-optimization-argument)
endif()

add_executable(ozo_request_callback request_callback.cpp)
target_link_libraries(ozo_request_callback ozo)

# enable a bunch of warnings and make them errors
target_compile_options(ozo_request_callback PRIVATE -Wall -Wextra -Wsign-compare -Werror)

# ignore specific errors for clang
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(ozo_request_callback PRIVATE -Wno-ignored-optimization-argument)
endif()

add_executable(ozo_retry_request_pool retry_request_pool.cpp)
target_link_libraries(ozo_retry_request_pool ozo)

# enable a bunch of warnings and make them errors
target_compile_options(ozo_retry_request_pool PRIVATE -Wall -Wextra -Wsign-compare -Werror)

# ignore specific errors for clang
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(ozo_retry_request_pool PRIVATE -Wno-ignored-optimization-argument)
endif()

add_executable(ozo_composite_types composite_types.cpp)
target_link_libraries(ozo_composite_types ozo)

# enable a bunch of warnings and make them errors
target_compile_options(ozo_composite_types PRIVATE -Wall -Wextra -Wsign-compare -Werror)

# ignore specific errors for clang
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(ozo_composite_types PRIVATE -Wno-ignored-optimization-argument)
endif()
