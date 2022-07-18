add_library(CallGraph MODULE src/CallGraph.cpp)

include_directories(include)

target_compile_features(CallGraph PRIVATE cxx_range_for cxx_auto_type)

set_target_properties(CallGraph PROPERTIES
        COMPILE_FLAGS "-g"
        )

if(APPLE)
    set_target_properties(CallGraph PROPERTIES
            LINK_FLAGS "-undefined dynamic_lookup"
            )
endif(APPLE)
