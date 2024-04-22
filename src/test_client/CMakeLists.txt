file(GLOB TEST_CLIENT_SRC "*.cpp")

add_executable(test_client ${TEST_CLIENT_SRC})
target_link_libraries(test_client PUBLIC 
        network
        pthread
) 

