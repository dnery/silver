cmake_minimum_required(VERSION 2.8)
project(ChessEngine)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

set(CMAKE_C_FLAGS_DEBUG "-O0 -g -pedantic -Wconversion -Wcast-align -Wunreachable-code -ftrapv")

set(CMAKE_C_FLAGS_RELEASE "-O2 -march=native")

set(SOURCE_FILES
        src/stdafx.h
        src/0x88movegen.c
        src/0x88movegen.h
        src/algebraic.c
        src/algebraic.h
        src/ilist.c
        src/ilist.h
        src/llist.c
        src/llist.h
        src/sq0x88.c
        src/sq0x88.h
        src/zobkey.c
        src/zobkey.h
        src/main.c)

add_executable(ChessEngine ${SOURCE_FILES})
