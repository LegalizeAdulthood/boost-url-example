add_executable(url-tool main.cpp ReadMe.md)
target_link_libraries(url-tool PRIVATE redirect)
target_folder(url-tool "Tools")
