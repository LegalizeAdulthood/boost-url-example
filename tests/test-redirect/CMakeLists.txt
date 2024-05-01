include(GoogleTest)

find_package(GTest CONFIG REQUIRED)

add_executable(test-redirect test-redirect.cpp)
target_link_libraries(test-redirect PUBLIC redirect GTest::gtest_main)
target_folder(test-redirect "Tests")

gtest_discover_tests(test-redirect)
