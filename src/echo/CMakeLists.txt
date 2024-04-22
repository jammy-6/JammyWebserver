file(GLOB ECHO_SRC "*.cpp")

add_executable(echo_server ${ECHO_SRC})
target_link_libraries(echo_server PUBLIC 
        network
        pthread
) 
