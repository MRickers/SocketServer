add_executable(socketserver_test 
socketserver_test.cpp
${SOURCE_DIR}/thread_pool/thread_pool.cpp)

target_include_directories(socketserver_test PRIVATE ${PROJECT_INCLUDE})
target_link_libraries(socketserver_test PRIVATE Catch2::Catch2WithMain Threads::Threads)
