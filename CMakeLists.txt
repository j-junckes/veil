cmake_minimum_required(VERSION 3.27.4)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(veil CXX ASM_NASM)
add_subdirectory(../standard standard_build)

enable_language(ASM_NASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function(load_profile ISA PLATFORM)
  # Obtain sources for the ISA
  file(GLOB_RECURSE ISA_SRCS "arch/${ISA}/*.cpp" "arch/${ISA}/*.asm")
  file(GLOB_RECURSE PLATFORM_SRCS "arch/${ISA}/${PLATFORM}/*.cpp" "arch/${ISA}/${PLATFORM}/*.asm")

  # Remove the files in PLATFORM_SRCS from ISA_SRCS
  list(REMOVE_ITEM ISA_SRCS ${PLATFORM_SRCS})
 
  # Load flags associated with ISA and Profile
  include("arch/${ISA}/flags.cmake")
  include("arch/${ISA}/${PLATFORM}/flags.cmake")
 
  # Now export our output variables
  set(ISA_SRCS ${ISA_SRCS} PARENT_SCOPE)
  set(PLATFORM_SRCS ${PLATFORM_SRCS} PARENT_SCOPE)
  set(PLATFORM_LAYOUT "${CMAKE_CURRENT_SOURCE_DIR}/arch/${ISA}/${PLATFORM}/layout.ld" PARENT_SCOPE)
 
  # And specific flags
  set(ISA_C_FLAGS ${ISA_C_FLAGS} PARENT_SCOPE)
  set(ISA_CXX_FLAGS ${ISA_CXX_FLAGS} PARENT_SCOPE)
  set(ISA_ASM_FLAGS ${ISA_ASM_FLAGS} PARENT_SCOPE)
  set(ISA_LINKER_FLAGS ${ISA_LINKER_FLAGS} PARENT_SCOPE)

  set(PLATFORM_C_FLAGS ${PLATFORM_C_FLAGS} PARENT_SCOPE)
  set(PLATFORM_CXX_FLAGS ${PLATFORM_CXX_FLAGS} PARENT_SCOPE)
  set(PLATFORM_ASM_FLAGS ${PLATFORM_ASM_FLAGS} PARENT_SCOPE)
  set(PLATFORM_LINKER_FLAGS ${PLATFORM_LINKER_FLAGS} PARENT_SCOPE)
  # ...
endfunction(load_profile)

load_profile("x86_64" "pc")

file(GLOB_RECURSE GENERIC_SRCS "src/*.cpp")

add_executable(veil ${PLATFORM_SRCS} ${ISA_SRCS} ${GENERIC_SRCS})
target_link_libraries(veil stdk)

target_include_directories(veil PUBLIC "include")

message(STATUS "Platform sources: ${PLATFORM_SRCS}")
message(STATUS "ISA sources: ${ISA_SRCS}")

set(CMAKE_ASM_NASM_COMPILE_OBJECT 
  "<CMAKE_ASM_NASM_COMPILER> ${ISA_ASM_FLAGS} ${PLATFORM_ASM_FLAGS} -o <OBJECT> <SOURCE>")
set(CMAKE_CXX_FLAGS "${ISA_CXX_FLAGS} ${PLATFORM_CXX_FLAGS}")
set(CMAKE_C_FLAGS "${ISA_C_FLAGS} ${PLATFORM_C_FLAGS}")
set_target_properties(veil PROPERTIES LINK_FLAGS 
  "-T ${PLATFORM_LAYOUT} -N ${ISA_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS}")
set_target_properties(veil PROPERTIES LINKER_LANGUAGE CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/output)

add_custom_command(
  TARGET veil  # Specify the target whose output you want to copy
  POST_BUILD   # Specify that this command should run after the build of the target
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:veil> ${EXECUTABLE_OUTPUT_PATH}/x86_64/boot/veil.kernel
)
