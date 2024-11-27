file(GLOB SQL_POOL_SRC "*.cpp")

add_library(sql_pool ${SQL_POOL_SRC})
target_link_libraries(sql_pool PUBLIC 
    mysqlcppconn
    log)