add_executable(EightQueen "EightQueen.cpp")
add_executable(DEvA::EightQueen ALIAS EightQueen)
target_link_libraries(EightQueen PUBLIC DEvA::DEvA)
file(COPY "./EASetup.json" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})