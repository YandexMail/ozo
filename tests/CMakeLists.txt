find_program(CCACHE_FOUND ccache)

include(ExternalProject)
ExternalProject_Add(
    GoogleTest
    GIT_REPOSITORY "https://github.com/google/googletest.git"
    GIT_TAG release-1.10.0
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
    UPDATE_COMMAND ""
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)
ExternalProject_Get_Property(GoogleTest source_dir)
include_directories(SYSTEM "${source_dir}/include")
include_directories(SYSTEM "${CMAKE_CURRENT_BINARY_DIR}/include")
link_directories("${CMAKE_CURRENT_BINARY_DIR}/lib")

include(sanitaizers)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(SOURCES
    impl/async_connect.cpp
    binary_deserialization.cpp
    binary_query.cpp
    binary_serialization.cpp
    bind.cpp
    composite.cpp
    connection.cpp
    connection_info.cpp
    connection_pool.cpp
    query_builder.cpp
    query_conf.cpp
    type_traits.cpp
    concept.cpp
    result.cpp
    none.cpp
    deadline.cpp
    error.cpp
    impl/async_send_query_params.cpp
    impl/async_get_result.cpp
    detail/base36.cpp
    detail/begin_statement_builder.cpp
    detail/functional.cpp
    detail/timeout_handler.cpp
    detail/make_copyable.cpp
    impl/request_oid_map.cpp
    impl/request_oid_map_handler.cpp
    impl/async_start_transaction.cpp
    impl/async_end_transaction.cpp
    transaction_status.cpp
    impl/async_request.cpp
    io/size_of.cpp
    failover/retry.cpp
    failover/strategy.cpp
    failover/role_based.cpp
    detail/deadline.cpp
    impl/cancel.cpp
    transaction.cpp
    main.cpp
)

if(OZO_BUILD_PG_TESTS)
    set(SOURCES
        ${SOURCES}
        integration/result_integration.cpp
        integration/request_integration.cpp
        integration/get_connection_integration.cpp
        integration/execute_integration.cpp
        integration/transaction_integration.cpp
        integration/retry_integration.cpp
        integration/cancel_integration.cpp
        integration/role_based_integration.cpp
        integration/connection_pool_integration.cpp
    )
    add_definitions(-DOZO_PG_TEST_CONNINFO="${OZO_PG_TEST_CONNINFO}")
endif()

add_executable(ozo_tests ${SOURCES})
add_dependencies(ozo_tests GoogleTest)
target_link_libraries(ozo_tests gtest)
target_link_libraries(ozo_tests gmock)
target_link_libraries(ozo_tests ozo)
add_test(ozo_tests ozo_tests)

if(CCACHE_FOUND)
    set_target_properties(ozo_tests PROPERTIES RULE_LAUNCH_COMPILE ccache)
    set_target_properties(ozo_tests PROPERTIES RULE_LAUNCH_LINK ccache)
endif()

# enable useful warnings and errors
target_compile_options(ozo_tests PRIVATE -Wall -Wextra -pedantic -Werror)

# ignore specific errors for clang
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(ozo_tests PRIVATE -Wno-ignored-optimization-argument)
endif()

if(OZO_COVERAGE AND CMAKE_COMPILER_IS_GNUCXX)
    include(CodeCoverage)
    APPEND_COVERAGE_COMPILER_FLAGS()
    set(COVERAGE_EXCLUDES "'.*/(tests|contrib|gmock|gtest)/.*'")
    SETUP_TARGET_FOR_COVERAGE_GCOVR(
        NAME ozo_coverage
        EXECUTABLE ctest -V
        DEPENDENCIES ozo_tests
    )
endif()
