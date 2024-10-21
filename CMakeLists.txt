cmake_minimum_required(VERSION 3.8)

# 프로젝트 이름을 package.xml과 일치시킵니다
project(my_canopen_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
# 필요한 다른 패키지를 추가하세요
# find_package(Easy_CANopen REQUIRED)

# Easy-CANopen 라이브러리 경로 설정
set(EASY_CANOPEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Easy_CANopen)

include_directories(
  ${EASY_CANOPEN_DIR}/Src
  ${EASY_CANOPEN_DIR}/Src/Easy_CANopen
)

# Easy-CANopen 라이브러리의 소스 파일만 수집합니다
file(GLOB_RECURSE EASY_CANOPEN_SOURCES "${EASY_CANOPEN_DIR}/Src/*.c" "${EASY_CANOPEN_DIR}/Src/*.cpp")
list(REMOVE_ITEM EASY_CANOPEN_SOURCES "${EASY_CANOPEN_DIR}/Src/main.c")
file(GLOB_RECURSE EASY_CANOPEN_HEADERS "${EASY_CANOPEN_DIR}/Src/*.h" "${EASY_CANOPEN_DIR}/Src/*.hpp")

# Easy_CANopen 라이브러리를 정적 라이브러리로 추가
add_library(Easy_CANopen STATIC ${EASY_CANOPEN_SOURCES} ${EASY_CANOPEN_HEADERS})

# 라이브러리에 필요한 종속성이 있다면 추가하세요
# 예: target_link_libraries(Easy_CANopen PUBLIC <dependency>)

# 실행 파일을 추가할 때, 라이브러리의 소스 파일을 포함하지 않습니다
add_executable(canopen_node src/canopen_node.cpp)

# 실행 파일에 rclcpp 종속성을 추가합니다
ament_target_dependencies(canopen_node rclcpp)

# 필요한 include 디렉토리를 설정합니다
target_include_directories(canopen_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${EASY_CANOPEN_DIR}/Src
  ${EASY_CANOPEN_DIR}/Src/Easy_CANopen
)

# C++ 표준 설정
target_compile_features(canopen_node PUBLIC c_std_99 cxx_std_17)

# 라이브러리를 실행 파일에 링크합니다
target_link_libraries(canopen_node
  Easy_CANopen
)

# 실행 파일을 설치 위치에 복사합니다
install(TARGETS
  canopen_node
  DESTINATION lib/${PROJECT_NAME}
)

# 테스트 관련 설정
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # 다음 줄은 저작권 관련 린터를 건너뜁니다
  set(ament_cmake_copyright_FOUND TRUE)
  # 다음 줄은 cpplint을 건너뜁니다 (git repo에서만 작동)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

# 패키지 설정
ament_package()
