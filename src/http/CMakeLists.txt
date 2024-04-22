file(GLOB HTTP_SRC "*.cpp")

add_executable(http_server ${HTTP_SRC})
target_link_libraries(http_server PUBLIC
        buffer
        log
        network
        pthread
        sql_pool
) 
