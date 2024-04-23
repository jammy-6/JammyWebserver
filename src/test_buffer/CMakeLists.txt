file(GLOB TEST_BUFFER_SRC "*.cpp")

add_executable(test_buffer ${TEST_BUFFER_SRC})
target_link_libraries(test_buffer PUBLIC 
        buffer
) 
