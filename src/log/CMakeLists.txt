file(GLOB LOG_SRC "*.cpp")

add_library(log ${LOG_SRC})
target_link_libraries(log PUBLIC buffer)