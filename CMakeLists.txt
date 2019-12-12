cmake_minimum_required( VERSION "2.9.0" )

project( nanodb CXX C )

find_package(PkgConfig REQUIRED)
pkg_search_module(OPENSSL REQUIRED openssl)


if( OPENSSL_FOUND )
    include_directories(
        src
        ${OPENSSL_INCLUDE_DIRS}
    )
    link_directories( ${OPENSSL_LIBRARY_DIRS} ) 
else()
    # Error; with REQUIRED, pkg_search_module() will throw an error by it's own
endif()


set( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../build )
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../build )


set( NANODB_SOURCES
    src/Connection.cc
    src/ListeningSocket.cc
)


set( NANODB_LIBS
    crypto
    ${OPENSSL_LIBRARIES}
)


set( NANODB_COMPILE_OPTIONS
    -Wall
    -Wextra
    -O3   
    -g
    -std=c++17
)



add_executable( nanodb ${NANODB_SOURCES} src/main.cc )

target_link_libraries( nanodb -Wl,-rpath=/usr/local/lib/nanodb:. ${NANODB_LIBS} )

set_target_properties( nanodb
    PROPERTIES
    COMPILE_OPTIONS  "${NANODB_COMPILE_OPTIONS}"
)

