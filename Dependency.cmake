# ExternalProject 관련 명령어 셋 추가
include(ExternalProject)

# Dependency 관련 변수 설정
set(DEP_INSTALL_DIR ${PROJECT_BINARY_DIR}/install) # 다운로드 한 프로젝트를 저장하는 위치
set(DEP_INCLUDE_DIR ${DEP_INSTALL_DIR}/include) # 필요한 헤더 파일
set(DEP_LIB_DIR ${DEP_INSTALL_DIR}/lib) # 라이브러리 파일

# spdlog: fast logger library
ExternalProject_Add(
    dep_spdlog # 프로젝트 이름
    GIT_REPOSITORY "https://github.com/gabime/spdlog.git" # 프로젝트 git 사이트
    GIT_TAG "v1.x" # 프로젝트 버전(브렌치나 태그 이름을 적는다.)
    GIT_SHALLOW 1 # git shallow - 가장 최신의 것만을 다운받게 하는 설정 On(1)/Off(0)
    UPDATE_COMMAND "" # 업데이트 관련 커맨드
    PATCH_COMMAND "" # 추가 수정을 위한 커맨드
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR} # Cmake 인자 - 어디에 이 프로젝트를 저장할 것인지 지정
    TEST_COMMAND "" # 테스트 관련 커맨드
)
# Dependency 리스트 및 라이브러리 파일 리스트 추가
set(DEP_LIST ${DEP_LIST} dep_spdlog)
set(DEP_LIBS ${DEP_LIBS} spdlog$<$<CONFIG:Debug>:d>)

# glfw
ExternalProject_Add(
    dep_glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG "3.3.8"
    GIT_SHALLOW 1
    UPDATE_COMMAND "" PATCH_COMMAND "" TEST_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
        -DGLFW_BUILD_EXAMPLES=OFF # example들은 가져오지 않음
        -DGLFW_BUILD_TESTS=OFF # test 파일도 안 가져옴
        -DGLFW_BUILD_DOCS=OFF # 문서 파일도 안 가져옴
    )
set(DEP_LIST ${DEP_LIST} dep_glfw)
set(DEP_LIBS ${DEP_LIBS} glfw3)

# glad
ExternalProject_Add(
    dep_glad
    GIT_REPOSITORY "https://github.com/Dav1dde/glad"
    GIT_TAG "v0.1.36"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
        -DGLAD_INSTALL=ON
    TEST_COMMAND ""
    )
set(DEP_LIST ${DEP_LIST} dep_glad)
set(DEP_LIBS ${DEP_LIBS} glad)

# stb
ExternalProject_Add(
    dep_stb
    GIT_REPOSITORY "https://github.com/nothings/stb"
    GIT_TAG "master"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CONFIGURE_COMMAND "" # 빌드를 따로 하지 않음
    BUILD_COMMAND ""
    TEST_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_BINARY_DIR}/dep_stb-prefix/src/dep_stb/stb_image.h
        ${DEP_INSTALL_DIR}/include/stb/stb_image.h
    )
set(DEP_LIST ${DEP_LIST} dep_stb)

# glm
ExternalProject_Add(
  dep_glm
  GIT_REPOSITORY "https://github.com/g-truc/glm"
  GIT_TAG "0.9.9.8"
  GIT_SHALLOW 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  TEST_COMMAND ""
  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_BINARY_DIR}/dep_glm-prefix/src/dep_glm/glm
    ${DEP_INSTALL_DIR}/include/glm
  )
set(DEP_LIST ${DEP_LIST} dep_glm)

add_library(imgui
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
    )
target_include_directories(imgui PRIVATE ${DEP_INCLUDE_DIR})
add_dependencies(imgui ${DEP_LIST})
set(DEP_INCLUDE_DIR ${DEP_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(DEP_LIST ${DEP_LIST} imgui)
set(DEP_LIBS ${DEP_LIBS} imgui)