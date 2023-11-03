#include "context.h"

#include <spdlog/spdlog.h>
#include <glad/glad.h> // glfw 추가 이전에 쓸 것
#include <GLFW/glfw3.h>

// 창의 크기가 바뀌는 이벤트 처리
void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    // 그림을 그릴 화면의 위치, 크기를 지정
    glViewport(0, 0, width, height);
}

// 키 이벤트 처리
void OnKeyEvent(GLFWwindow* window,
    int key, int scancode, int action, int mods) {
    // 누른 키의 정보를 출력
    SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}",
        key, scancode,
        action == GLFW_PRESS ? "Pressed" :
        action == GLFW_RELEASE ? "Released" :
        action == GLFW_REPEAT ? "Repeat" : "Unknown",
        mods & GLFW_MOD_CONTROL ? "C" : "-",
        mods & GLFW_MOD_SHIFT ? "S" : "-",
        mods & GLFW_MOD_ALT ? "A" : "-");
    // ESC키를 누르면 창을 종료한다.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main(int argc, const char **argv){
    SPDLOG_INFO("Start Program");

    // glfw 라이브러리 초기화, 실패하면 에러 출력후 종료
    SPDLOG_INFO("Initialize glfw");
    if (!glfwInit()) {
        const char* description = nullptr;
        glfwGetError(&description); // 에러가 발생한 이유를 가져옴
        // 에러 사유 출력
        SPDLOG_ERROR("failed to initialize glfw: {}", description);
        return -1;
    }

    // OpenGL 버전 설정
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw 윈도우 생성, 실패하면 에러 출력후 종료
    SPDLOG_INFO("Create glfw window");
    // 윈도우 생성
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (!window) {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        return -1;
    }
    // OpenGL context를 사용할 것을 명시
    glfwMakeContextCurrent(window);

    // glad를 활용한 OpenGL 함수 로딩
    // glfwMakeContextCurrent 이후에 수행해야 함
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_ERROR("failed to initialize glad");
        glfwTerminate();
        return -1;
    }
    auto glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    SPDLOG_INFO("OpenGL context version: {}", glVersion);

    // context로 쉐이더와 프로그램 동시에 생성
    auto context = Context::Create();
    // 생성 실패한 경우
    if (!context) {
        SPDLOG_ERROR("failed to create context");
        glfwTerminate();
        return -1;
    }

    // 호출할 콜백 함수 지정
    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT); // 창 크기 지정
    // 프레임 버퍼(창)의 크기가 바뀔 때 실행할 함수
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
    // 키가 눌렸을 때 실행할 함수
    glfwSetKeyCallback(window, OnKeyEvent);

    // glfw 루프 실행, 윈도우 close 버튼을 누르면 정상 종료
    SPDLOG_INFO("Start main loop");
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // 키보드, 마우스 등 각종 이벤트 수집

        // 렌더링 
        context->Render();
        
        // 프론트/백 버퍼 교체
        glfwSwapBuffers(window);
    }
    // 메모리 해제
    context.reset();

    glfwTerminate();
    return 0;
}

// Source : https://www.youtube.com/watch?v=mJ8JuPR7ehw&list=PLvNHCGtd4kh_cYLKMP_E-jwF3YKpDP4hf&index=4