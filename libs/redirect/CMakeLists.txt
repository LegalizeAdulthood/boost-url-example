add_library(redirect
    include/redirect/redirect.h
    include/redirect/Rewrite.h
    Rewrite.cpp
)
find_package(Boost REQUIRED COMPONENTS url)
target_include_directories(redirect PUBLIC include)
target_link_libraries(redirect PUBLIC Boost::url)
target_folder(redirect "Libraries")
