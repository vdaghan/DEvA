add_executable(EightQueeen "EightQueen.cpp")
add_executable(DEvA::EightQueeen ALIAS EightQueeen)
target_link_libraries(EightQueeen PUBLIC DEvA::DEvA)
