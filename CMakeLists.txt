cmake_minimum_required(VERSION 3.3)
project(ChessEngine)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wextra")

set(SOURCE_FILES
        source/0x88movegen.c
        source/afx/0x88movegen.h
        source/algebraic.c
        source/afx/algebraic.h
        source/ilist.c
        source/afx/ilist.h
        source/llist.c
        source/afx/llist.h
        source/main.c
        source/sq0x88.c
        source/afx/sq0x88.h
        source/afx/stdafx.h
        source/zobkey.c
        source/afx/zobkey.h)

add_executable(ChessEngine ${SOURCE_FILES})
