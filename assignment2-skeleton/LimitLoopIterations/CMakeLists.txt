add_library(LimitLoopIterations MODULE src/LimitLoopIterations.cpp)

include_directories(include)

target_compile_features(LimitLoopIterations PRIVATE cxx_range_for cxx_auto_type)

set_target_properties(LimitLoopIterations PROPERTIES
        COMPILE_FLAGS "-g"
        )

if(APPLE)
    set_target_properties(LimitLoopIterations PROPERTIES
            LINK_FLAGS "-undefined dynamic_lookup"
            )
endif(APPLE)
