add_library(redirect
    include/redirect/redirect.h
    redirect.cpp
)
target_include_directories(redirect PUBLIC include)
target_folder(redirect "Libraries")
