#!/bin/bash -ex

scripts/build.sh pg docker clang release

docker-compose up -d ozo_postgres ozo_proxy

function run_example {
    BINARY="\${BASE_BUILD_DIR}/clang_release/examples/${1:?}"
    CONN_INFO='"host=${POSTGRES_HOST:?} user=${POSTGRES_USER:?} dbname=${POSTGRES_DB:?} password=${POSTGRES_PASSWORD:?}"'
    docker-compose run \
        --rm \
        --user "$(id -u):$(id -g)" \
        ozo_build_with_pg_tests \
        bash \
        -exc "/code/scripts/wait_postgres.sh; ${BINARY:?} ${CONN_INFO:?} ${CONN_INFO:?}"
}

function run_example_proxy {
    BINARY="\${BASE_BUILD_DIR}/clang_release/examples/${1:?}"
    CONN_INFO='"host=${PROXY_HOST:?} user=${POSTGRES_USER:?} dbname=${POSTGRES_DB:?} password=${POSTGRES_PASSWORD:?}"'
    docker-compose run \
        --rm \
        --user "$(id -u):$(id -g)" \
        ozo_build_with_pg_tests \
        bash \
        -exc "/code/scripts/wait_proxy.sh; ${BINARY:?} ${CONN_INFO:?} ${CONN_INFO:?}"
}

run_example ozo_request_coroutine
run_example ozo_connection_pool
run_example ozo_retry_request
run_example ozo_role_based_request
run_example ozo_transaction
run_example ozo_request_future
run_example ozo_request_callback
run_example_proxy ozo_retry_request_pool
run_example ozo_composite_types

docker-compose stop ozo_postgres ozo_proxy
docker-compose rm -f ozo_postgres ozo_proxy
