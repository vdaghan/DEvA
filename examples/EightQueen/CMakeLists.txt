add_executable(EightQueen "EightQueen.cpp")
add_executable(DEvA::EightQueen ALIAS EightQueen)
target_link_libraries(EightQueen PUBLIC DEvA::DEvA)
add_custom_command(TARGET EightQueen PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                "${CMAKE_CURRENT_SOURCE_DIR}/EASetup.json"
                ${CMAKE_CURRENT_BINARY_DIR})
