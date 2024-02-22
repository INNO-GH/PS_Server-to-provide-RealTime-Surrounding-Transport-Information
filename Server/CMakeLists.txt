cmake_minimum_required(VERSION 3.0)
project(Server)

# 컴파일러 옵션 설정
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# libcurl 라이브러리 찾기
find_package(CURL REQUIRED)

# 소스 파일 목록
set(SOURCES
    Communicator.cpp
    Reader.cpp
    Seoul_Organizer.cpp
    Daegu_Organizer.cpp
)

# 실행 파일 생성
add_executable(Server ${SOURCES})

# libcurl 라이브러리 링크
target_link_libraries(Server CURL::libcurl)
