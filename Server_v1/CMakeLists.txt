cmake_minimum_required(VERSION 2.8.4)
set(MY_PROJECT Server_v1)
project(${MY_PROJECT})
set(MY_COMPILE_FLAGS "-std=c++11 -pthread")

# inner Cmake vars set
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${MY_COMPILE_FLAGS}")

# source
set(SOURCE_FILES
                    main.cpp)
add_executable(${MY_PROJECT} ${SOURCE_FILES})

# lib
set(LIBEVENT levent)
add_library(${LIBEVENT} STATIC IMPORTED)
set_property(TARGET ${LIBEVENT} PROPERTY
             IMPORTED_LOCATION /usr/local/lib/libevent.a)

set(LIBEVENT_PTHREADS levent_pthreads)
add_library(${LIBEVENT_PTHREADS} STATIC IMPORTED)
set_property(TARGET ${LIBEVENT_PTHREADS} PROPERTY
                          IMPORTED_LOCATION /usr/local/lib/libevent_pthreads.a)

set(LIBRARIES
                ${LIBEVENT}
                ${LIBEVENT_PTHREADS})

target_link_libraries(${MY_PROJECT} ${LIBRARIES})
