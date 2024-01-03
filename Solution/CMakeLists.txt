cmake_minimum_required(VERSION 3.18)
project(Solution)

add_subdirectory("${PROJECT_SOURCE_DIR}/../Common" "${PROJECT_SOURCE_DIR}/Common/output")
add_subdirectory("${PROJECT_SOURCE_DIR}/../Client" "${PROJECT_SOURCE_DIR}/Client/output")
add_subdirectory("${PROJECT_SOURCE_DIR}/../Server" "${PROJECT_SOURCE_DIR}/Server/output")
