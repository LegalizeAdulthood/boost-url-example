find_package(Boost REQUIRED COMPONENTS url)

add_executable(url-tool main.cpp)
target_link_libraries(url-tool PRIVATE Boost::url)
target_folder(url-tool "Tools")
