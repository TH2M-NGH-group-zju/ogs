append_source_files(SOURCES)
append_source_files(SOURCES BHE)
append_source_files(SOURCES BoundaryConditions)
append_source_files(SOURCES LocalAssemblers)

ogs_add_library(HeatTransportBHE ${SOURCES})

target_link_libraries(HeatTransportBHE
    PUBLIC
        ProcessLib
    PRIVATE
    ParameterLib
    $<$<TARGET_EXISTS:pybind11::pybind11>:pybind11::pybind11>
)

if(BUILD_TESTING)
    include(Tests.cmake)
endif()
