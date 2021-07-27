include_directories(include/)

# add dynamic link library
add_library(SkeletonPass MODULE src/SkeletonPass.cpp)

# add statically linked library
add_library(SkeletonPassStatic STATIC src/SkeletonPass.cpp)

set_target_properties(SkeletonPass PROPERTIES
        COMPILE_FLAGS "-g"
        )

set_target_properties(SkeletonPassStatic PROPERTIES
        COMPILE_FLAGS "-g"
        )

target_compile_features(SkeletonPass PRIVATE cxx_range_for cxx_auto_type)

target_compile_features(SkeletonPassStatic PRIVATE cxx_range_for cxx_auto_type)