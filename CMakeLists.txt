cmake_minimum_required(VERSION 2.8)
project(MATRIX_ORAM)

set(CMAKE_CXX_STANDARD 17)

add_library(matrix_src
    src/Block.cpp
    src/Zmq_socket.cpp
    src/MatrixORAM.cpp
    src/ClientMatrixORAM.cpp
    src/ServerMatrixORAM.cpp
)

add_executable(MATRIX_ORAM main.cpp)
target_link_libraries(MATRIX_ORAM matrix_src -ltomcrypt -lzmq)
# target_link_libraries(MATRIX_ORAM matrix_src -lzmq)
