set(SOURCES CreateMFront.cpp CreateMFront.h)

list(APPEND SOURCES MFront.cpp MFront.h)

ogs_add_library(MaterialLib_SolidModels_MFront ${SOURCES})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      "${PROJECT_SOURCE_DIR}/ThirdParty/MGIS")
mfront_behaviours_check_library(
    OgsMFrontBehaviour
    BDT
    DruckerPrager
    Elasticity
    Lubby2
    Lubby2mod
    MohrCoulombAbboSloan
    MohrCoulombAbboSloanAniso
    MohrCoulombAbboSloanOrtho
    MohrCoulombAbboSloanUBI
    MohrCoulombAbboSloanUBIOrtho
    StandardElasticityBrick
    StandardElasticityBrickOrtho)

target_link_libraries(
    MaterialLib_SolidModels_MFront
    PUBLIC BaseLib NumLib OgsMFrontBehaviour
    PRIVATE MathLib MeshLib spdlog::spdlog)

# Disable warnings for generated OgsMFrontBehaviour
target_compile_options(
    OgsMFrontBehaviour
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-w>
        $<$<CXX_COMPILER_ID:MSVC>:/W0>)

target_include_directories(MaterialLib_SolidModels_MFront
                           PUBLIC ThirdParty/MGIS/include)
target_link_libraries(MaterialLib_SolidModels_MFront
                      PUBLIC MFrontGenericInterface)
target_compile_definitions(MaterialLib_SolidModels_MFront
                           PRIVATE OGS_USE_MFRONT)
