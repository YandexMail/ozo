#!/bin/bash -e

TARGET=$2

build_clang() {
    COMPILER=clang
    CC_COMPILER=clang
    CXX_COMPILER=clang++
    case "$TARGET" in
        debug)
            CMAKE_BUILD_TYPE=Debug
            build
        ;;
        release)
            CMAKE_BUILD_TYPE=RelWithDebInfo
            build
        ;;
        test_external_project)
            CMAKE_BUILD_TYPE=Release
            OZO_TEST_EXTERNAL_PROJECT=ON
            build
        ;;
        asan)
            export ASAN_OPTIONS='halt_on_error=1:strict_string_checks=1:detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1'
            ASAN_CXX_FLAGS='-fno-omit-frame-pointer -fsanitize=address -fsanitize-address-use-after-scope -fsanitize-recover=address'
            CMAKE_CXX_FLAGS_RELWITHDEBINFO="-g -O1 ${ASAN_CXX_FLAGS}"
            CMAKE_BUILD_TYPE=RelWithDebInfo
            build
        ;;
        ubsan)
            export UBSAN_OPTIONS='print_stacktrace=1'
            CMAKE_BUILD_TYPE=Debug
            CMAKE_CXX_FLAGS='-fno-omit-frame-pointer -fsanitize=undefined'
            build
        ;;
        tsan)
            export TSAN_OPTIONS='second_deadlock_stack=1'
            CMAKE_BUILD_TYPE=RelWithDebInfo
            CMAKE_CXX_FLAGS_RELWITHDEBINFO='-g -O2 -fno-omit-frame-pointer -fsanitize=thread'
            build
        ;;
        conan)
            build_conan
        ;;
        *) usage "bad clang target '$TARGET'";;
    esac
}

build_gcc() {
    COMPILER=gcc
    CC_COMPILER=gcc
    CXX_COMPILER=g++
    case "$TARGET" in
        debug)
            CMAKE_BUILD_TYPE=Debug
            build
        ;;
        release)
            CMAKE_BUILD_TYPE=RelWithDebInfo
            build
        ;;
        test_external_project)
            CMAKE_BUILD_TYPE=Release
            OZO_TEST_EXTERNAL_PROJECT=ON
            build
        ;;
        coverage)
            CMAKE_BUILD_TYPE=Debug
            OZO_COVERAGE=ON
            build
        ;;
        conan)
            build_conan
        ;;
        *) usage "bad gcc target '$TARGET'";;
    esac
}

build_docs() {
    doxygen
}

usage() {
    local NAME=`basename $0`
    echo $NAME: ERROR: $* 1>&2
    echo 'usage:
        '$NAME' all
            build all targets for all compilers
        '$NAME' docs
            build documentation
        '$NAME' <compiler> <target>
        Build with specified compiler or target
        compiler : gcc | clang
        target   :
            - for gcc   : debug | release | test_external_project | coverage | conan
            - for clang : debug | release | test_external_project | asan | ubsan | tsan | conan
        '$NAME' docker [all | docs | <compiler> <target>]
        Build inside Docker
        '$NAME' pg [docker] [all | <compiler> <target>]
        Build with PostgreSQL integration tests' 1>&2
    exit 1
}

build_all() {
    $0 gcc debug
    $0 gcc release
    $0 gcc coverage
    $0 gcc test_external_project
    $0 gcc conan
    $0 clang debug
    $0 clang release
    $0 clang asan
    $0 clang ubsan
    $0 clang tsan
    $0 clang test_external_project
    $0 clang conan
}

build_conan() {
    echo "CONAN_USER_HOME: ${CONAN_USER_HOME}"
    if ! [[ "${CONAN_USER_HOME}" ]]; then
        CONAN_USER_HOME=conanh
    fi

    mkdir -p "${CONAN_USER_HOME}"
    export CONAN_USER_HOME="$(readlink -f "${CONAN_USER_HOME}")"
    conan profile new default --detect --force

    conan create contrib/resource_pool
    conan create .
}

build() {
    echo "BASE_BUILD_DIR: ${BASE_BUILD_DIR}"
    if ! [[ "${BASE_BUILD_DIR}" ]]; then
        BASE_BUILD_DIR=build
    fi
    SOURCE_DIR=${PWD}
    BUILD_DIR=${BASE_BUILD_DIR}/${COMPILER}_${TARGET}
    mkdir -p ${BUILD_DIR}
    cd ${BUILD_DIR}
    cmake \
        -DCMAKE_C_COMPILER_LAUNCHER=ccache \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
        -DCMAKE_C_COMPILER="${CC_COMPILER}" \
        -DCMAKE_CXX_COMPILER="${CXX_COMPILER}" \
        -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS"\
        -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="$CMAKE_CXX_FLAGS_RELWITHDEBINFO" \
        -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE \
        -DOZO_BUILD_TESTS=ON \
        -DOZO_BUILD_EXAMPLES=ON \
        -DOZO_BUILD_BENCHMARKS=ON \
        -DOZO_COVERAGE=$OZO_COVERAGE \
        -DOZO_BUILD_PG_TESTS=$OZO_BUILD_PG_TESTS \
        -DOZO_PG_TEST_CONNINFO="host=${POSTGRES_HOST} port=5432 dbname=${POSTGRES_DB} user=${POSTGRES_USER} password=${POSTGRES_PASSWORD}" \
        -DBoost_NO_SYSTEM_PATHS="${OZO_Boost_NO_SYSTEM_PATHS}" \
        -DBOOST_ROOT="${OZO_BOOST_ROOT}" \
        -DBOOST_LIBRARYDIR="${OZO_BOOST_LIBRARYDIR}" \
        ${SOURCE_DIR}
    make -j $(nproc || sysctl -n hw.ncpu)
    if [[ $OZO_BUILD_PG_TESTS == "ON" ]]; then
        ${SOURCE_DIR}/scripts/wait_postgres.sh
    fi
    if [[ ${OZO_COVERAGE} == "ON" ]]; then
        make ozo_coverage
    else
        ctest -V
    fi
    if [[ ${OZO_TEST_EXTERNAL_PROJECT} == "ON" ]]; then
        INSTALL_DIR="${BUILD_DIR}/ozo_install"
        mkdir -p ${INSTALL_DIR}
        make DESTDIR=${INSTALL_DIR} install

        PREVIOUS_DIR="$(pwd)"
        cd ${INSTALL_DIR}
        OZO_ROOT_DIR="$(pwd)/usr/local"
        cd ${PREVIOUS_DIR}


        EXT_BUILD_DIR="${BUILD_DIR}_external_project"
        mkdir -p ${EXT_BUILD_DIR}
        cd ${EXT_BUILD_DIR}
        cmake \
            -DCMAKE_C_COMPILER_LAUNCHER=ccache \
            -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
            -DCMAKE_C_COMPILER="${CC_COMPILER}" \
            -DCMAKE_CXX_COMPILER="${CXX_COMPILER}" \
            -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
            -Dozo_ROOT="${OZO_ROOT_DIR}" \
            ${SOURCE_DIR}/tests/external_project
        make
    fi
}

launch_in_docker() {
    if [[ "${OZO_BUILD_PG_TESTS}" == "ON" ]]; then
        SERVICE=ozo_build_with_pg_tests
    else
        SERVICE=ozo_build
    fi
    if ! [[ "${BASE_BUILD_DIR}" ]]; then
        export BASE_BUILD_DIR=build
    fi
    BASE_BUILD_DIR=${BASE_BUILD_DIR}/docker
    docker-compose run --rm \
        --user "$(id -u):$(id -g)" \
        ${SERVICE} $0 $*
}

launch_with_pg() {
    docker-compose up -d ozo_postgres
    ID=$(docker-compose ps -q ozo_postgres)
    export POSTGRES_HOST=$(docker inspect --format '{{ .NetworkSettings.Networks.ozo_ozo.IPAddress }}' ${ID})
    export POSTGRES_DB=ozo_test_db
    export POSTGRES_USER=ozo_test_user
    export POSTGRES_PASSWORD='v4Xpkocl~5l6h219Ynk1lJbM61jIr!ca'
    export OZO_BUILD_PG_TESTS=ON
    export BASE_BUILD_DIR=build/pg
    $*
    docker-compose stop ozo_postgres
    docker-compose rm -f ozo_postgres
}

case "$1" in
    gcc|clang|all|docs)
        set -x
        build_${1}
        exit 0
    ;;
    docker)
        set -x
        launch_in_docker $2 $3 $4 $5
        exit 0
    ;;
    pg)
        set -x
        launch_with_pg $0 $2 $3 $4 $5
        exit 0
    ;;
    help|-h|--help)
        usage
    ;;
    *)
        usage "bad first parameter '$1'"
    ;;
esac
