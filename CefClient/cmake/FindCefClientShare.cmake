set(CEF_CLIENT_SHARE_INCLUDE_DIR ${PROJECT_ROOT}/CefClient_share/include)

if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    link_directories(
        ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO}
        )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    link_directories(
        ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE}
        )
endif()
