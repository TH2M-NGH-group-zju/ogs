add_executable(xdmfdiff xdmfdiff.cpp)
target_link_libraries(xdmfdiff OgsXdmf std::filesystem)
install(PROGRAMS $<TARGET_FILE:xdmfdiff> DESTINATION bin COMPONENT ogs_extras)
