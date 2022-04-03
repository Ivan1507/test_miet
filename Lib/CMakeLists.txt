file(GLOB SRCS Kernels/ref/*.c Kernels/ref/*.cpp) 
add_library(openvx_kernels ${SRCS})
target_include_directories(openvx_kernels PUBLIC Common .)
