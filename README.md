# OpenGL Example

- 이 코드는 아래 유튜브에 있는 강원대학교 2021-1학기 그래픽스 강의 수업을 들으며 따라한 코드를 올려 놓은 곳입니다.
- <https://www.youtube.com/playlist?list=PLvNHCGtd4kh_cYLKMP_E-jwF3YKpDP4hf>

- 좋은 수업과 자료를 모든 사람을 위해 공개해 주신 권지용 교수님께 감사드립니다.
- 텍스쳐와 3D 모델은 원본 튜토리얼인 <https://learnopengl.com> 에서 가져온 것입니다.
- PBR 텍스쳐는 <https://freepbr.com/materials/rusted-iron-pbr-metal-material-alt/> 에서 가져왔습니다.
- HDR 맵은 <https://web.archive.org/web/20200609124833/http://www.hdrlabs.com/sibl/archive.html>의 Alexs_Apt_2k.hdr 파일을 가져왔습니다. 현재 원본 링크가 없어졌으므로 여기에는 Wayback Machine을 이용한 대체 링크를 걸어두었고, hdr 파일은 강의의 github 페이지 에서 가져 왔습니다. -> <https://github.com/rinthel/opengl_course/blob/main/projects/15_pbr/pbr_example/image/Alexs_Apt_2k.hdr>
- Dependency.cmake 파일은 windows 운영체제를 기준으로 작성되었고, 다른 운영체제에서는 그대로 사용시 실행되지 않을 수도 있습니다.
- 사용된 라이브러리들은 그 라이브러리의 최신 버전이 아닌 강의의 버전과 동일한 것을 사용했습니다. 최신 버전의 라이브러리에서는 이 코드가 동작하지 않을 수도 있습니다.
- 현재 8-2 코드에는 오타가 있습니다. 실행은 되지만 빛이 이상하게 동작할 수 있는데, 8-2 commit에 어떤 오타가 있는지 메모가 있으므로 참고해서 고치면 문제 없을 것입니다.
- OpenGL로 텍스쳐 없는 2D 도형을 그리는 방법은 Hazel 게임 엔진 개발자인 The Cherno님의 튜토리얼 <https://www.youtube.com/watch?v=W3gAzLwfIP0&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2>의 내용을 참고했습니다.
- OpenGL 오류 처리 방법은 <https://www.youtube.com/watch?v=FBbPWSOQ0-w&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&index=10>을 참고했습니다.

## 내용 정리 노트 - <https://rinthel.github.io/opengl_course> 참고

### OpenGL로 텍스쳐 없는 2D 도형을 그리는 방법 (이 부분은 강의 노트가 아닌 learnopengl.com을 참고함)

#### 1. glfw 창 생성

- glfw 초기화

```c++
if (!glfwInit()) {
    const char* description = nullptr;
    glfwGetError(&description); // 에러가 발생한 이유를 가져옴
    // 에러 사유 출력
    std::cout << "failed to initialize glfw : " << description << std::endl;
    return -1;
}
```

- opengl 버전 설정

```c++
// OpenGL 버전 설정
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
```

- glfw 윈도우, OpenGL context 생성

```c++
// glfw 윈도우 생성, 실패하면 에러 출력후 종료
auto window = glfwCreateWindow(창 가로 크기, 창 세로 크기, 창 이름, nullptr, nullptr);
if (!window) {
    std::cout << "failed to create glfw window" << std::endl;
    glfwTerminate();
    return -1;
}
// OpenGL context를 사용할 것을 명시
glfwMakeContextCurrent(window);
```

#### 2. glad 불러오기 : glfwMakeContextCurrent 이후에 수행해야 함

```c++
// glad를 활용한 OpenGL 함수 로딩
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "failed to initialize glad" << std::endl;
    glfwTerminate();
    return -1;
}
```

#### 3. VAO, VBO, EBO 제작

- (1) VAO 생성 / 바인딩
- glGenVertexArrays(개수, &ID) : VAO 생성
- glBindVertexArray(ID) : VAO 바인딩

- (2) VBO, EBO 생성, vertex data와 index data를 복사 : 반드시 VAO 바인딩 후 할 것
- glGenBuffers(개수, &ID) : 버퍼 생성(VBO, EBO)동일
- glBindBuffer(타입, ID) : 버퍼 바인딩
  - VBO는 GL_ARRAY_BUFFER, EBO는 GL_ELEMENT_ARRAY_BUFFER를 사용한다.
- glBufferData(타입, 크기, 데이터 배열, 데이터 관리 방식) : 정점이나 인덱스 데이터를 해당 버퍼에 복사
- ex) glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  - 4번째 인자는 그래픽 카드가 주어진 데이터를 관리하는 방식에 대한 것으로 GL_(접근의 정도)_(접근 특성)의 형태로 이루어져 있다.

| 접근의 정도 | 설명 |
|--|--|
| STATIC | 버퍼의 데이터는 한 번 수정되어 여러 번 사용된다. |
| DYNAMIC | 버퍼의 데이터는 반복적으로 수정되고 여러 번 사용된다. |
| STREAM | 버퍼의 데이터는 한 번 수정되고 최대 몇 번만 사용된다. |

| 접근 특성 | 설명 |
|--|--|
| DRAW | 버퍼의 데이터는 애플리케이션에 의해 수정되며 OpenGL에서 무언가를 그리거나 이미지 관련 처리를 할 때에 사용된다. |
| READ | 버퍼의 데이터는 OpenGL에서 데이터를 읽어 수정되고 애플리케이션에서 데이터를 요청할 때 이 버퍼의 데이터는 반환된다. |
| COPY | 버퍼의 데이터는 OpenGL에서 데이터를 읽어 수정되고 OpenGL에서 무언가를 그리거나 이미지 관련 처리를 할 때에 사용된다. |

- (3) 정점 속성 지정 / 연결
- glEnableVertexAttribArray(정점 속성 번호) : 해당 번호의 정점 속성을 사용할 수 있도록 설정
- glVertexAttribPointer(정점 속성 번호, 속성을 이루는 변수의 개수, 변수의 종류, 정규화 여부, 바이트 크기, 시작 위치) : 정점 속성을 지정해 줌

- 1번째 인자에는 속성의 인덱스를 써준다. 예를 들어 한 정점에 위치, 색깔 정보가 있다면 위치는 0, 색깔은 1이 된다.(나열한 순서대로)
- 2번째 인자에는 속성의 크기(바이트가 아님! 그 정보를 이루는 수의 개수)를 써준다. 2D 좌표나 텍스쳐 정점 좌표의 경우 2, 3D 좌표나 색깔은 3을 써주면 된다.
- 3번째 인자에는 저장하는 수의 타입을 써준다. `GL_BYTE`, `GL_UNSIGNED_BYTE`, `GL_SHORT`, `GL_UNSIGNED_SHORT`, `GL_INT`,  `GL_UNSIGNED_INT`, `GL_HALF_FLOAT`, `GL_FLOAT`, `GL_DOUBLE` 등을 써서 저장된 수의 타입을 지정해 줄 수 있다.
- 4번째 인자에는 GL_TRUE, GL_FALSE 중 하나를 넣는다. GL_TRUE를 하게 되면 값들을 -1 ~ 1사이로 정규화를 시킨다. (이 경우에는 이미 -1 ~ 1사이에 있으므로 GL_FALSE를 사용한다.)
- 5번째 인자에는 한 정점을 나타내는 데이터의 총 크기를 써준다. 한 정점에 3D좌표와 색깔을 정보로 담았다면 이 인자에는 6 * sizeof(float)이 들어가면 된다.
- 6번째 인자에는 그 한 정점의 데이터 안에서 해당 정보가 몇 바이트부터 위치하는 지 시작 위치를 써준다.(바이트 단위)

```c++
float vertices[] =
{//       정점 위치         /    정점의 색깔   / 텍스쳐 정점 좌표
    -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,   0.0f, 0.0f, // 왼쪽 아래 모서리
    -0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // 왼쪽 위 모서리
     0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // 오른쪽 위 모서리
     0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,   1.0f, 0.0f  // 오른쪽 아래 모서리
};

// 정점 위치
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
// 정점 색깔
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);
// 텍스쳐 정점 좌표
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
glEnableVertexAttribArray(2);
```

- (4) 버퍼 바인딩 해제

```c++
glBindBuffer(GL_ARRAY_BUFFER, 0);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
```

#### 4. 쉐이더 컴파일 & 프로그램 생성

- (1) 쉐이더 생성
- glCreateShader() : 타입에 맞는 쉐이더 생성
  - glCreateShader(GL_FRAGMENT_SHADER) : fragment shader
  - glCreateShader(GL_VERTEX_SHADER) : vertex shader
- glShaderSource(ID, count, &shaderSource, NULL) : 쉐이더 소스가 들어있는 문자열 전달
- glCompileShader(ID) : 쉐이더 컴파일
- glGetShaderiv(ID, GL_COMPILE_STATUS, &success) : 컴파일 성공 여부 확인
  - success가 1이 아닌 경우 실패 : glGetShaderInfoLog(ID, 오류 문자열 길이, NULL, 문자열을 담을 변수)로 원인 파악 가능

```c++
// Shader 클래스의 LoadFile 함수의 일부이다.
// create and compile shader
// 쉐이더를 만들고 ID 저장
m_shader = glCreateShader(shaderType);
// 소스 파일 가져옴 - (ID, 개수, 코드(const char*), 길이)
glShaderSource(m_shader, 1, (const GLchar* const*)&codePtr, &codeLength);
glCompileShader(m_shader);

// check compile error
int success = 0;
// 쉐이더의 정보(여기서는 컴파일 성공 여부)를 얻어옴
glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
if (!success) {
    // 컴파일에 실패한 경우 에러 메시지 출력
    char infoLog[1024];
    glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog);
    SPDLOG_ERROR("failed to compile shader: \"{}\"", filename);
    SPDLOG_ERROR("reason: {}", infoLog);
}
```

- (2) 프로그램 생성
- glCreateProgram() : 프로그램 생성
- glAttachShader(program ID, shader ID) : 컴파일 된 쉐이더를 프로그램에 삽입
- glLinkProgram(ID) : 프로그램 링크
- glGetProgramiv(ID, GL_LINK_STATUS, &success) : 링크 성공 여부 확인
  - success가 1이 아닌 경우 실패 : glGetProgramInfoLog(ID, 오류 문자열 길이, NULL, 문자열을 담을 변수)로 원인 파악 가능

```c++
// Program 클래스의 Link 함수의 일부이다.
// 프로그램 생성
m_program = glCreateProgram();
// 쉐이더들을 프로그램에 붙임
for (auto& shader: shaders)
    glAttachShader(m_program, shader->Get());
// 프로그램 링크
glLinkProgram(m_program);

// 프로그램 링크가 성공적으로 이루어졌는지 판단
int success = 0;
glGetProgramiv(m_program, GL_LINK_STATUS, &success);
// 실패한 경우 오류 출력
if (!success) {
    char infoLog[1024];
    glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
    SPDLOG_ERROR("failed to link program: {}", infoLog);
    return false;
}
```

#### 5. glfw 콜백 함수 지정 / 창 초기화

- glClearColor(float r, float g, float b, float a) : 화면 배경색 지정
- glfwSetFramebufferSizeCallback(window, 콜백 함수 이름) : 창의 크기가 변할 때 호출할 함수 지정
콜백 함수는 아래처럼, glfw window, 창의 크기를 인자로 받는다

```c++
void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    // 그림을 그릴 화면의 위치, 크기를 지정
    glViewport(0, 0, width, height);
}
```

- glfwSetKeyCallback(window, 콜백 함수 이름) : 어떤 키가 눌렸을 때 호출할 함수 지정
콜백 함수는 아래처럼 glfw window, key : 키 종류, scancode, action : 누름, 계속 누르고 있음, 뗌 상태, mods : ctrl, alt, shift를 동시에 추가로 눌렀는가? 를 인자로 받는다.

```c++
// 키 이벤트 처리
void OnKeyEvent(GLFWwindow* window,
    int key, int scancode, int action, int mods) {
    // ESC키를 누르면 창을 종료한다.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
```

#### 6. 메인 루프

- while 루프의 조건은 !glfwWindowShouldClose(window)로 지정한다.
- glClear(GL_COLOR_BUFFER_BIT) : 화면 초기화
- glfwPollEvents() : 이벤트 처리

- 그림 그리기
- glUseProgram(program ID) : 지정된 쉐이더 프로그램 사용 준비
- glDrawElements(primitive type, EBO 변수 개수, 변수 타입, 오프셋) : 버퍼 데이터 대로 화면에 그림을 그린다. : glUseProgram 후에 사용
ex) glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
- glfwSwapBuffers(window) : 프론트 버퍼와 백 버퍼를 교체 : 렌더링 코드 맨 마지막에 있어야 함

```c++
/* 창을 닫을 때까지 반복 - 메인 루프 
여기서는 이해를 위해 context 클래스가 없는 버전을 가져왔다.
원래 코드에서는 context->Render();에 렌더링 부분이 모두 들어있다.
*/
while (!glfwWindowShouldClose(window))
{
    /* 여기에 렌더링 부분을 넣음 */
    glClear(GL_COLOR_BUFFER_BIT);
    /* 삼각형을 그림(정점 2 * 3개) */
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    /* 백 버퍼와 프론트 버퍼 교체 */
    glfwSwapBuffers(window);
    /* 이벤트 처리 */
    glfwPollEvents();
}
```

#### 7. 종료, 메모리 정리

```c++
// 쉐이더 프로그램과 버퍼 삭제
glDeleteProgram(program ID);
glDeleteBuffers(1, &VBO ID);
glDeleteBuffers(1, &EBO ID);
glfwTerminate();
```

### GLSL 기초

- GLSL은 OpenGL에서 shader를 작성하기 위해 제공하는 C 기반의 언어이다.
- 대략적인 형태

```glsl
#version 330 core

in type in_var_name;

out type out_var_name;

uniform type uniform_name;

void main(){
    out_var_name = ...;
}
```

- 쉐이더는 버전을 정해주는 것으로 시작한다. 이 예제에서는 3.3 core profile을 사용하므로 #version 330 core를 써주었다.
- in으로 시작하는 변수는 vertex shader인 경우 VBO의 데이터에서 부터, fragment shader의 경우 vertex shader로 부터 받는 데이터를 뜻한다.
- out으로 시작하는 변수는 vertex shader인 경우 fragment shader에게 전달해주는 변수, fragment shader의 경우 픽셀의 최종 색상을 뜻한다. fragment shader에서는 보통 out vec3 fragColor;의 형태로 짓는다.
- uniform으로 시작하는 변수는 우리가 작성하는 c++프로그램에서 직접 값을 전달받는 변수를 뜻한다.
- C언어 처럼 main() 함수 안에서 시작된다. main() 함수에서 연산을 수행한 뒤, 반드시 out으로 지정한 변수의 값을 모두 지정해 준 뒤 함수가 끝이 난다.

#### GLSL type

- 기본 타입 : int, float, double, uint, bool
- 벡터 타입 : vecN : float, bvecN : bool, ivecN : int, uvecN : uint, dvecN : double (N에는 2, 3, 4가 들어감)
- 행렬 타입 : matN : float, bmatN : bool, imatN : int, umatN : uint, dmatN : double (N에는 2, 3, 4가 들어감)
- mat4는 4x4 크기이고, 각 원소가 float인 행렬을 의미한다.

#### Vector 값에 접근 / 초기화

- 벡터이름.x or .y or .z or .w를 통해 벡터의 1, 2, 3, 4번째 원소에 접근할 수 있다.
- .rgba(색상), .stpq(텍스쳐)를 이용해도 1~4번째 원소에 접근할 수 있다.
- swizzling : .뒤에 변수를 여러개 써서 단일 값 뿐만 아니라 vec2, vec3, vec4의 형태로 가져올 수 있다.

```glsl
vec2 some;
vec3 foo = some.xyy;
vec4 bar = foo.wzyx;
vec3 color = bar.rgb;
```

- 벡터 값의 초기화는 vec2(), vec3(), vec4() 형태의 생성자를 사용할 수 있다. 이때 이 안에서도 swizzling을 쓸 수 있다.

```glsl
vec2 some = vec2(0.2f, 0.6f);
vec3 foo = vec3(some.xy, 0.7f);
vec4 bar = vec4(foo.xy, some.xy);
vec3 color = vec3(bar.xy, 1.0f);
```

#### Vertex shader의 기본 형태

```glsl
#version 330 core
layout (location = 0) in vec3 aPos; // 0번째 attribute가 정점의 위치

out vec4 vertexColor; // fragment shader로 넘어갈 컬러값

void main() {
    gl_Position = vec4(aPos, 1.0); // 정점의 화면상 위치 계산
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // fragment shader에 값 전달
}
```

- vertex shader에서는 각 정점별로 설정된 정점 속성 (vertex attribute)들을 입력 받는다. (glEnableVertexAttribArray 함수로 지정한 것들이다.) 이때 layout (location = 속성 번호)를 통해 몇 번째 속성을 입력받을 지를 결정할 수 있다. 아래의 경우에는 attribute가 위치, 색깔 2개일 때 지정하는 방식이다.

```glsl
layout (location = 0) in vec3 aPos; // 0번째 attribute가 정점의 위치
layout (location = 1) in vec3 aColor; // 1번째 attribute가 정점의 색깔
```

- main 함수에서 gl_Position 변수에 값을 지정해주는 것이 vertex shader의 가장 중요한 역할이다. 이 변수는 GLSL에 사전 정의된 함수로 화면 상에서 해당 정점이 위치할 곳을 뜻한다. 그리고 main 함수 안에서는 fragment shader에 넘겨 줄 out 변수의 값을 지정해주는 것도 필요하다.

#### Fragment shader의 기본 형태

```glsl
#version 330 core

in vec4 vertexColor; // vertex shader로부터 입력된 변수 (같은 변수명, 같은 타입)
out vec4 FragColor; // 최종 출력 색상

void main() {
    FragColor = vertexColor;
}
```

- fragment shader에서는 Rasterization 과정을 거쳐 픽셀별로 할당된 vertex shader의 출력 변수를 받아서 그 픽셀의 최종적인 색상을 결정해 준다.
- vertex shader에서의 out 변수와 타입과 이름이 같은 in 변수를 받고, out 변수로는 픽셀의 색상을 vec4의 형태로 반환한다.
- 이후 학습할 빛, 그림자, 텍스쳐 연산은 모두 여기에서 이루어 진다.

#### Uniform 변수 선언과 값 할당

- uniform 변수는 앞에서 서술했듯이 쉐이더 외부에서 값을 받아올 수 있는 global 변수이고, 쉐이더 안에서는 uniform으로 시작하는 변수 형태로 정의한다.
- 이제 쉐이더 바깥에서 이 uniform 변수에 값을 전달하려면 우선, glUseProgram()으로 프로그램을 사용해준다.
- 그 다음, glGetUniformLocation(program ID, uniform 변수 이름)을 사용해서 해당 uniform의 주소를 불러온다. uniform 변수 이름은 반드시 shader에서 선언했던 이름과 똑같아야 한다. 가져오는데 성공했으면 임의의 주소값(0 이상)이, 실패했으면 -1이 들어간다.
- uniform의 주소를 불러오는 데 성공했다면 해당 uniform 변수의 타입에 맞는 glUniform..(주소, ...) 함수를 호출해서 원하는 값을 전달해 줄 수 있다. float인 경우 glUniform1f ~ glUniform4f 함수를, int인 경우 glUniform1i ~ glUniform4i 함수를, uint인 경우 glUniform1ui ~ glUniform4ui 함수를 쓸 수 있다.

```c++
// 프로그램에서 color uniform 변수의 위치를 가져옴
auto loc = glGetUniformLocation(m_program->Get(), "color");
// 해당 위치에 값을 넣음(color는 vec4이다.)
glUniform4f(loc, 1.0f, 1.0f, 0.0f, 1.0f);
```

- 이렇게 uniform 변수를 사용하면 c++ 프로그램에서 계산한 값을 쉐이더에 직접 전달함으로써 렌더링 루프에서 매 프레임마다 프로그램 사용 후 다른 값을 uniform 변수에 전달함으로써 프레임마다 달라지는 애니메이션을 만들어 낼 수도 있다.

```c++
void Context::Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    float time = 0.0f;
    float t = sinf(time) * 0.5f + 0.5f;
    auto loc = glGetUniformLocation(m_program->Get(), "color");
    m_program->Use();
    glUniform4f(loc, t, 1.0f - t, 1.0f, 1.0f);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    time += 0.016f;
}
```

- m_program->Get()은 쉐이더 프로그램의 ID를 가져오는 것이고, m_program->Use()는 glUseProgram(ID)와 같다.

### 텍스쳐

- 정점의 수를 늘리고 정점들의 색을 달리 지정해 주면 더 높은 퀄리티의 도형이나 모델을 만들 수 있지만 각 정점들에 색을 지정하는 것만으로는 높은 수준의 3D 모델을 만들어내기 어렵고 정점의 수가 늘어날 수록 비용도 커지게 된다. 그래서 면에 이미지를 입혀서 적은 비용으로 디테일한 렌더링을 할 수 있도록 하는 것이 텍스쳐이다.

#### 텍스쳐 관련 개념

##### 1. 텍스쳐 좌표

- 텍스쳐를 도형에 입히기 위해서는 해당 정점에서 텍스쳐의 어느 부분을 가져올 지를 정해야 한다. 그래서 OpenGL에서는 아래 그림처럼 왼쪽 하단을 (0, 0), 오른쪽 상단을 (1, 1)로 하고 오른쪽으로 갈수록 x좌표가 증가, 위쪽으로 갈수록 y좌표가 증가하는 텍스쳐 좌표계를 사용한다.
![texture1](/note_image/texture1.png)

- 정점의 텍스쳐 좌표는 (2차원 기준으로) 정점 속성의 형태로 s, t 형태의 2개의 float 형태로 입력된다. vertex shader에서 계산된 텍스쳐 좌표는 rasterization을 거친 뒤 fragment shader에서 해당 좌표에 해당하는 픽셀을 텍스쳐에서 가져오게 되므로써 텍스쳐를 면에다가 그릴 수 있게 된다.

##### 2. texture Wrapping

- 정점에는 0 ~ 1 범위를 벗어난 텍스쳐 좌표의 값도 입력될 수 있다. 이 경우 범위 밖의 픽셀을 어떻게 지정할 지를 결정해야 하는데 이때 사용하는 것이 texture wrapping이다.
- Texture wrapping에는 원래 텍스쳐를 벗어난 범위에 계속 이어붙인 형태의 GL_REPEAT, GL_REPEAT와 비슷하지만 거울 형태로 이어붙인 GL_MIRRORED_REPEAT, 텍스쳐의 가장자리 픽셀을 연장한 GL_CLAMP_TO_EDGE, 범위를 벗어난 모든 픽셀을 지정한 색상으로 바꾸는 GL_CLAMP_TO_BORDER가 있다.

##### 3. texture filtering

- 우리가 텍스쳐로 사용할 이미지는 화면의 크기와 달라 화면보다 크거나 작은 경우가 많을 것이다. 이때 픽셀의 색을 결정할 때 텍스쳐 이미지에 있는 좌표들을 어떻게 사용할 것인지를 지정하는 것이 texture filtering 이다.

- GL_NEAREST를 지정하면, 해당 텍스쳐 좌표에서 가장 가까운 픽셀의 색을 그대로 들고오게 된다.
- GL_LINEAR를 지정하면, 해당 텍스쳐 좌표에서 인접한 4개의 픽셀의 색의 평균을 가져오게 된다.

#### OpenGL에서 텍스쳐 사용하기

##### 1. texture object 생성 / 바인딩

- glGenTextures(개수, &ID) 함수를 이용해서 텍스쳐 오브젝트를 생성한다.
- glBindTexture(타입, ID)로 텍스쳐 바인딩을 해서 사용할 수 있게 만든다. 타입에는 GL_TEXTURE_2D를 넣는다.

```c++
uint32_t m_texture;
glGenTextures(1, &m_texture);
glBindTexture(GL_TEXTURE_2D, m_texture);
```

##### 2. 텍스쳐의 wrapping과 filtering을 설정한다

- glTexParameteri(타입, 설정, 설정값)로 현재 바인딩된 텍스쳐의 wrapping과 filtering을 설정할 수 있다.
- GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MAG_FILTER를 사용해서 텍스쳐가 확대, 축소 되었을 때 어떤 filter를 사용할 지를 각각 지정해 줄 수 있다.
- GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T를 사용해서 x, y 좌표가 0 ~ 1의 범위를 벗어났을 때 어떤 방식으로 wrapping을 해줄지를 결정할 수 있다.

```c++
// filtering
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// wrapping
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
```

- 만들려고 하는 텍스쳐가 2개 이상인 경우 ID를 저장할 수 있는 여러 개의 uint32_t 변수를 생성하고 특정 텍스쳐를 설정하고자 할 때 그 텍스쳐의 ID를 glBindTexture()에 넣어 바인딩한 뒤 설정을 해주면 된다.

##### 3. 이미지 데이터를 GPU로 복사한다

- 우선 이미지 데이터를 가져와야 한다. stb library를 사용하면 이미지를 OpenGL에서 사용할 수 있는 형태로 불러올 수 있다.
- stbi_set_flip_vertically_on_load()를 사용한 이유는 stb에서는 이미지를 좌측 상단부터 읽어오지만 OpenGL에서 텍스쳐 좌표는 왼쪽 아래에서 시작하기 때문에 상하가 반전되기 때문이다.
- stbi_load(파일 경로, &너비, &높이, &채널 수, 오프셋) 함수를 이용해서 이미지를 불러올 수 있다. 반환형은 uint8_t*이고, 너비, 높이, 채널 수(RGB는 3, RGBA는 4)가 자동으로 지정된다.

```c++
// Image 클래스에서 stb를 사용하는 부분을 들고옴
bool Image::LoadWithStb(const std::string& filepath) {
    stbi_set_flip_vertically_on_load(true);
    m_data = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channelCount, 0);
    if (!m_data) {
        SPDLOG_ERROR("failed to load image: {}", filepath);
        return false;
    }
    return true;
}
```

- glTexImage2D(target, level, internalFormat, width, height, border, format, type, data)를 사용해서 바인딩된 텍스쳐에 이미지 데이터를 넣어줄 수 있다.
- target에는 텍스쳐 타입(GL_TEXTURE_2D), level에는 mipmap level (0이 기본, 1씩 늘어날 때마다 크기가 반이 됨), internelFormat에는 만들어 줄 `텍스쳐`의 이미지 포맷이다.
- width, height에는 이미지의 크기가 들어간다. stbi_load를 사용해서 얻어온 값을 쓴다. border에는 텍스쳐 외각에 지정할 보더의 크기를 지정해준다.
- format에는 가져오는 `이미지`의 포맷을 넣어준다. type에는 이미지를 이루는 각 채널이 어떤 값으로 이루어져 있는지를 지정한다. png 파일에는 GL_UNSIGNED_BYTE를 지정해준다. 마지막으로 data에는 이미지 데이터를 가지고 있는 uint8_t* 포인터를 넣어주면 된다.

- internalFormat, format에 들어가는 '포맷'에는 `GL_RED`(빨강색만 존재), `GL_RG`(빨강, 초록), `GL_RGB`(빨강, 초록, 파랑), `GL_RGBA`(RGB + 투명도) 등이 있다. 더 자세한 내용은 <https://www.khronos.org/opengl/wiki/Image_Format> 참고

```c++
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
    image->GetWidth(), image->GetHeight(), 0,
    GL_RGB, GL_UNSIGNED_BYTE, image->GetData());
```

##### 4. 쉐이더에 사용하고자 하는 texture의 ID를 uniform 변수 형태로 전달한다

- VBO에서 정점 속성에 텍스쳐 좌표에 관한 정보를 추가로 지정해주어야 한다.

```c++
// 정점 데이터
float vertices[] = {
    // x, y, z(정점 위치), r, g, b(정점 색깔), s, t(정점의 텍스쳐 좌표)
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
};
```

- 정점 데이터가 바뀌었으므로, 정점 속성에 텍스쳐 좌표를 저장하는 2번 속성이 추가되어야 한다.

```c++
// 2. VBO 바인딩 / 정점 데이터 복사
// 마지막 인자에 곱해지는 값은 정점 리스트의 데이터 크기이다.
m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 32);

// 3. VAO에서 정점 데이터의 속성을 지정
// 5번째 인자에 곱해지는 값은 한 정점 당 데이터 크기이다.(지금은 위치 3개, 색깔 3개, 텍스쳐 2개로 총 8개)
m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0); // 위치(0-2)
m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 3); // 색상(3-5)
m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 6); // 텍스쳐 좌표(6-7)
```

- CreateWithData() 함수에는 glGenBuffers, glBindBuffer, glBufferData 함수가 내장되어 있고, SetAttrib() 함수에는 glEnableVertexAttribArray, glVertexAttribPointer 함수가 내장되어 있다.
- 이제 쉐이더에서 새로운 속성인 텍스쳐 좌표를 받아야 한다. vertex shader에서는 aTexCoord라는 이름으로 텍스쳐 좌표를 받아오고 vec2의 형태로 fragment shader에 전달해준다.

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 vertexColor;
out vec2 texCoord;

void main() {
    gl_Position = vec4(aPos, 1.0);
    vertexColor = vec4(aColor, 1.0);
    texCoord = aTexCoord; // 텍스쳐 좌표를 fragment shader에 전달
}
```

- fragment shader에서는 vertex shader에서 받은 텍스쳐 좌표를 이용해서, 텍스쳐에서 해당 좌표에 대응하는 픽셀을 가져온다. texture 함수에 만들어진 텍스쳐 오브젝트의 ID와 텍스쳐 좌표를 전달하면 색상을 계산해 낼 수 있다.

```glsl
#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;

void main() {
    fragColor = texture(tex, texCoord); // 텍스쳐에서 픽셀을 가져온다.
}
```

- 이제 uniform 변수인 tex의 값을 전달해 주면 된다. glActiveTexture(GL_TEXTUREX)를 이용해서 특정 번호의 텍스쳐를 활성화 한 뒤, glBindTexture(GL_TEXTURE_2D, ID)로 해당 텍스쳐에 텍스쳐 오브젝트를 바인딩 해준다. glActiveTexture에 넣어준 인자에서 X에는 0 ~ 31의 수가 들어가고, ID에 대응하는 텍스쳐는 반드시 위의 1 ~ 3을 모두 수행한 것이어야 한다. 그 후 쉐이더 프로그램을 use 한 뒤, glUniform1i와 glGetUniformLocation 함수를 이용해서 uniform 변수에 방금 바인딩한 텍스쳐 슬롯의 번호를 넣어준다.(텍스쳐 오브젝트의 ID가 아니다.)

```c++
// 텍스쳐를 텍스쳐 슬롯 0번에 넣는다.
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, m_texture->Get());

// 프래그먼트 쉐이더에 바인딩을 할 텍스쳐의 텍스쳐 슬롯 번호를 Uniform으로 전달해준다.
m_program->Use();
glUniform1i(glGetUniformLocation(m_program->Get(), "tex"), 0);
```

##### 5. 사용 후 메모리 해제

- 텍스쳐 오브젝트는 glDeleteTextures(1, &ID)로 해제한다.
- 이미지는 stbi_image_free(pointer)로 해제한다.

#### Texture mipmap

- 화면에 그리는 텍스쳐 면이 원래 이미지의 크기보다 작아질 경우 화면에 1개의 픽셀을 그리는 데에 원래 텍스쳐의 여러 개의 픽셀이 관여하게 되고 그 결과 원래 이미지에서는 볼 수 없었던 무늬가 발생하는 경우가 있다. 그래서 원래의 이미지를 더 작게 그리게 될 것을 대비해서 원래 이미지에서 가로와 세로의 길이를 절반씩 줄인 더 작은 이미지들을 만들 수 있는데 이를 mipmap이라고 한다.
- 가장 큰 원래 이미지는 mipmap level이 0이고, 가로와 세로가 절반씩 줄때마다 레벨이 1씩 올라간다. 512x512의 이미지의 경우 256x256 크기가 1레벨, 128x128 크기가 2레벨, ... , 1x1 크기가 9레벨로 총 0 ~ 9단계까지 만들 수 있다.
- 더 작은 이미지들을 저장하기 위해 원래의 이미지의 1/3크기의 공간이 추가로 필요하다.
- OpenGL에서 mipmap을 사용하기 위해서는 텍스쳐에서 GL_TEXTURE_MIN_FILTER에 대한 필터를 지정할 때 GL_NEAREST_MIPMAP_NEAREST 또는 GL_LINEAR_MIPMAP_LINEAR를 전달해준다.
  - GL_NEAREST_MIPMAP_NEAREST는 화면에서 텍스쳐 면의 크기에 제일 가까운 밉맵을 1개만 고른 뒤, 그 안에서 텍스쳐 좌표에 가장 가까운 픽셀을 1개 골라서 지정한다는 뜻이다.
  - GL_LINEAR_MIPMAP_LINEAR는 화면에서 텍스쳐 면의 크기에 제일 가까운 밉맵 2개를 골라 그 둘을 이용해 선형 보간을 한 텍스쳐를 만들고, 그 텍스쳐에서 텍스쳐 좌표에 가장 가까운 픽셀 4개를 골라서 선형 보간한 색상을 지정한다는 뜻이다. (trilinear interpolation라고도 함)
- 그 후 glTexImage2D() 함수 이후에 glGenerateMipmap() 함수를 사용해서 밉맵을 만들 수 있다.

```c++
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

// ...

glGenerateMipmap(GL_TEXTURE_2D);
```

#### Texture Blending

- 2장 이상의 이미지를 하나의 면에다 혼합해서 넣는 방식을 texture blending이라고 한다.
- fragment shader에서 2개의 텍스쳐를 받은 뒤, 각각의 텍스쳐에서 얻어온 픽셀 값에 최대 합이 1이 되도록 0 ~ 1사이의 실수를 곱해주면 두 텍스쳐의 모습이 섞여서 하나의 면에 들어가게 된다.

```glsl
#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;
uniform sampler2D tex2;

void main() {
    fragColor = texture(tex, texCoord) * 0.6 +
        texture(tex2, texCoord) * 0.4; // 1번째 텍스쳐와 2번째 텍스쳐의 비율이 6 : 4가 되도록 혼합
}
```

- 2개 이상의 텍스쳐를 쉐이더에 올바르게 전달하기 위해서 위에서 사용했던 glActiveTexture + glBindTexture 함수를 사용해 줄 것이다. 두 함수를 써서 특정 슬롯의 텍스쳐를 활성화 하고, 그 텍스쳐에 이미지를 붙인 텍스쳐 오브젝트를 바인딩한 뒤, glGetUniformLocation + glUniform1i을 사용해서 텍스쳐 슬롯 번호를 쉐이더에 전달해준다.

```c++
// 두 텍스쳐를 각각 텍스쳐 슬롯 0, 1번에 넣는다.
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, m_texture->Get());
glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, m_texture2->Get());

// 프래그먼트 쉐이더에 바인딩을 할 두 텍스쳐의 텍스쳐 슬롯 번호를 Uniform으로 전달해준다.
m_program->Use();
glUniform1i(glGetUniformLocation(m_program->Get(), "tex"), 0);
glUniform1i(glGetUniformLocation(m_program->Get(), "tex2"), 1);
```

#### (+) 이미지(png) 생성하기

- stbi를 이용해 외부의 이미지를 불러오는 것이 아니라 코드 상에서 png 데이터를 직접 만들어서 OpenGL에 전달해 줄 수도 있다. width, height, channelCount를 지정해 준 뒤 width x height x channelCount 길이의 uint8_t를 원소로 하는 배열을 생성해서 data에 넣어준 뒤, 코드에서 data를 수정해 주고, 텍스쳐를 만드는 과정에서 glTexImage2D() 함수에 width, height와 마지막 9번째 인자에 data를 전달해 주면 된다.

```c++
// 이미지 데이터를 생성해 주는 함수
bool Image::Allocate(int width, int height, int channelCount) {
    m_width = width;
    m_height = height;
    m_channelCount = channelCount;
    m_data = (uint8_t*)malloc(m_width * m_height * m_channelCount);
    return m_data ? true : false;
}

// data 지정 함수의 예시, 여기서는 원 강의에서 사용한 체커보드를 만드는 함수를 가져왔다.
void Image::SetCheckImage(int gridX, int gridY) {
    for (int j = 0; j < m_height; j++) {
        for (int i = 0; i < m_width; i++) {
            int pos = (j * m_width + i) * m_channelCount;
            bool even = ((i / gridX) + (j / gridY)) % 2 == 0;
            uint8_t value = even ? 255 : 0;
            for (int k = 0; k < m_channelCount; k++)
                m_data[pos + k] = value;
            if (m_channelCount > 3)
                m_data[3] = 255;
        }
    }
}
```

### Transformation

- 화면에서 물체를 원하는 위치에 그리기 위해 VBO의 정점 위치 인자를 수정할 수 있지만 매번 그 수치를 바꾸는 것은 비효율적이다. 그래서 이 위치에 물체가 이동, 회전한 정보 등을 저장하는 추가적인 행렬을 곱한 뒤 그 행렬과 위치 벡터를 선형 변환을 사용하는 방법을 사용할 수 있다.

- 동차 좌표계 : 표현하고자 하는 차원수보다 1차원이 늘어난 벡터로 좌표를 표시하는 방법
- 3차원에서는 4번째 차원인 w를 추가해서 동차 좌표계를 표현한다.
- (wx, wy, wz, w)와 (x, y, z, 1)은 동일한 좌표를 표현한다.
- (x, y, z, 1)은 점을 의미하고, (x, y, z, 0)은 벡터를 의미한다.
- 이렇게 동차 좌표계를 사용하게 되면 평행이동과 원근 투영을 선형 변환으로 표현할 수 있다는 장점이 있다.

#### 3가지 transformation

##### 1. scaling

- `원점`을 기준으로 벡터의 크기를 확대하거나 축소한다.
- 대각 성분에 각 차원별로 몇 배를 조정할 지 배율을 써놓은 4x4 행렬로 표현한다.

##### 2. Translation

- 점을 각 축마다 지정한 수치만큼 이동시킨다.
- 대각 성분이 모두 1이고, 4번째 열에 x, y, z축으로 얼마만큼 이동할 지를 써준다.

##### 3. Rotation

- 회전의 경우 어떤 축을 기준으로 점들을 특정 각도만큼 회전시킨다.
- 어떤 축을 기준으로 하느냐에 따라 회전 행렬은 달라진다.


- 이 3가지 transformation을 한 번에 적용할 경우에는 확대 -> 회전 -> 평행이동 순으로 점에 선형 변환 적용

#### glm 라이브러리

- OpenGL math library로 3D 그래픽스에 필요한 4차원 선형대수 연산관련 기능을 제공하는 C++ 라이브러리 이다.
- glm 라이브러리를 cmake로 가져온 뒤 아래 세 헤더를 추가해서 사용할 수 있다. (이 예제에서는 common.h에 추가)

```c++
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
```

- translate로 평행이동, rotate로 회전, scale로 크기 변경 행렬을 만들어 낼 수 있다.
- 세 함수 모두 1번째 인자에 glm::mat4(1.0f)를 넣는다.
- translate의 경우 2번째 인자에 평행이동할 양을 glm::vec3(x, y, z)의 형태로 넣어주면 된다.
- scale의 경우 2번째 인자에 각 축마다 확대/축소할 비율을 glm::vec3(x, y, z)의 형태로 넣어주면 된다.
- rotate의 경우 2번째 인자에 회전할 각도를 써준다. 도를 라디안으로 바꾸기 위해 glm::radians(각도) 함수를 쓰면 된다. 그리고 3번째 인자에는 회전축을 써준다. glm::vec3(x, y, z)의 형태로 임의의 회전축을 지정해줄 수 있고, x, y, z축을 기준으로 하려면 (1, 0, 0), (0, 1, 0), (0, 0, 1)을 써주면 된다.
- 세 translation을 모두 지정한 후 확대 -> 회전 -> 평행이동 순으로 적용되도록 translation x rotation x scale x vec형태로 곱해서 대입한다. 이 완성된 행렬을 Model Matrix라고 한다.

```c++
// 위치 (1, 2, 3)의 점. 동차좌표계 사용
glm::vec4 vec(1.0f, 2.0f, 3.0f, 1.0f);
// 단위행렬 기준 (1, 1, 2)만큼 평행이동하는 행렬
auto trans = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 2.0f));
// 단위행렬 기준 y축으로 90도만큼 회전하는 행렬
auto rot = glm::rotate(glm::mat4(1.0f),
  glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
// 단위행렬 기준 모든 축에 대해 3배율 확대하는 행렬
auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f));
// 확대 -> 회전 -> 평행이동 순으로 점에 선형 변환 적용
vec = trans * rot * scale * vec;
```

#### 쉐이더에서 translation 반영하기

- 행렬 계산은 glm 라이브러리를 이용하고 vertex shader에서는 이 계산된 행렬을 uniform 변수로 받은 뒤, 이것을 정점 속성으로 받은 위치 속성 (aPos)와 곱해주면 된다.

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
[...]
 
uniform mat4 transform;
 
[...]
 
void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    [...]
}
```

- 이제 프로그램을 사용한 뒤 계산된 행렬을 glGetUniformLocation + glUniformMatrix4fv함수를 사용해서 mat4의 형태로 vertex shader에 전달해 줄 수 있다. glUniformMatrix4fv에는 glGetUniformLocation으로 얻은 uniform의 위치, 전달할 행렬 개수, transpose 여부(여기서는 GL_FALSE로 보냄), 데이터를 인자로 전달한다.
- glm::value_ptr()을 사용해서 쉐이더에서 인식할 수 있는 형태로 행렬을 보낼 수 있다.

```c++
// 0.5배 축소후 z축으로 90도 회전하는 행렬
auto transform = glm::rotate(
    glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)),
    glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
auto transformLoc = glGetUniformLocation(m_program->Get(), "transform");
glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
```

#### Coordinate System (좌표계)

- 어떤 정점의 위치를 기술하기 위한 기준이다.
- 선형 변환은 한 좌표계에 대해 기술된 벡터를 다른 좌표계에 대해 기술하는 과정이라고 할 수 있다.
- World space는 원점이 (0, 0, 0)이고, 회전이나 크기 변환이 되지 않은 기준 좌표계를 뜻한다.
- Local space는 특정 오브젝트를 기준으로 하는 좌표계를 뜻한다.
- View space는 세상을 바라보는 카메라를 기준으로 하는 좌표계를 뜻한다.
- Canonical space는 view space에서 화면에 보이는 영역인 -1.0 ~ 1.0 좌표만 남긴 것을 뜻한다. clip space라고도 한다.

#### 좌표계 변환

- 우리가 만든 오브젝트를 화면에 그리기 위해서는 물체 기준으로 작성된 Local space의 좌표를 최종적으로 화면의 좌표계인 Canonical space로 변환하는 것이 필요하다.

> 1. 우선, local space를 world space로 변환한다. 이 변환을 위해 사용되는 행렬을 Model Matrix라고 하고 위에서 언급한 translation 3가지를 모두 곱한 것이 바로 이 행렬이다.
> 2. 그 다음으로 world space를 카메라를 기준으로 한 view space로 변환한다. 이 변환을 위해 사용되는 행렬을 View Matrix라고 한다. 이 행렬은 카메라의 위치, 회전을 담고 있는 행렬이다.
> 3. 이제 view space를 canonical space로 변환한다. 이 변환을 위해 사용되는 행렬을 Projection Matrix라고 한다.
> 4. view space로 변환된 좌표를 clip space의 좌표로 투영한다. 이 좌표는 -1.0 ~ 1.0 범위로 처리되고, 이 범위 밖에 있는 vertex들은 그려지지 않게 된다.
> 5. 마지막으로 이 좌표들을 glViewport 함수를 통해 정의된 좌표 범위로 변환해서 screen space로 변환하면 끝이다. 이 좌표들은 rasterizer로 보내진다.

#### Orthogonal projection (직교 투영)

- 원근감 없이 평행한 선이 계속 평행하도록 투영하는 방식이다.
![transformation1-1](/note_image/transformation1-1.png)
- left, right, bottom, top, near, far의 6개의 파라미터를 받는다. left, right, bottom, top은 near plane과 far plane의 상하좌우 위치를 뜻한다. near, far는 near과 far plane이 카메라와 떨어진 거리를 뜻한다.
![transformation1-2](/note_image/transformation1-2.png)
- 아래와 같은 행렬이 projection matrix이다.

- $\begin{pmatrix} \frac{2}{r-l} & 0 & 0 & -\frac{r+l}{r-l} \\ 0 & \frac{2}{t-b} & 0 & -\frac{t+b}{t-b} \\ 0 & 0 & \frac{-2}{f-n} & -\frac{f+n}{f-n} \\ 0 & 0 & 0 & 1 \end{pmatrix}$

- z축에 - 기호가 있는 이유는 clip space에서는 오른손 좌표계가 왼손 좌표계로 바뀌기 때문이다.

#### Perspective projection (원근 투영)

- 변환 이전에 평행한 선이 변환 후에 한점에서 마난다. 이를 소실점이라고 한다.
- 멀리 있을 수록 물체가 작게 보이므로 원근감이 발생하게 된다.
![transformation2-1](/note_image/transformation2-1.png)

- 파라미터로는 aspect(종횡비), fov(Field of View), far, near를 받는다. 종횡비는 화면의 가로와 세로의 비율을 나타낸 것이고(screen_width / screen_height), fov는 보이는 화면 비율, 즉 far와 near plane이 얼마나 큰 지를 결정한다. 현실과 가까운 값을 주려면 45로 설정하면 된다.
![transformation2-2](/note_image/transformation2-2.png)
- 아래와 같은 행렬이 projection matrix이다.
- $\begin{pmatrix} \frac{1}{aspect \times \tan \left(\frac{fov}{2}\right)} & 0 & 0 & 0 \\ 0 & \frac{1}{\tan \left(\frac{fov}{2}\right)} & 0 & 0 \\ 0 & 0 & -\frac{far+near}{far-near} & -\frac{2 \times far \times near}{far-near} \\ 0 & 0 & -1 & 0 \end{pmatrix}$

#### Transformation 구현

- Transformation은 model, view, projection matrix를 모두 곱해서 물체의 local space를 clip space로 바꾸는 과정이다. 3개의 행렬을 모두 곱한 행렬을 MVP matrix라고 한다.
- vertex shader에 transform을 넘겨줄 때, MVP matrix를 넘겨주면 된다.

```c++
// Model matrix
auto model = glm::rotate(glm::mat4(1.0f),
  glm::radians(-50.0f), glm::vec3(1.0f, 0.0f, 0.0f));
// View matrix : 카메라는 원점으로부터 z축 방향으로 -3만큼 떨어짐
auto view = glm::translate(glm::mat4(1.0f),
  glm::vec3(0.0f, 0.0f, -3.0f));
// Projection matrix : 종횡비 4:3, 세로화각 45도의 원근 투영
auto projection = glm::perspective(glm::radians(45.0f),
  (float)640 / (float)480, 0.01f, 10.0f);
// Model -> View -> Projection 순으로 적용되게 곱함
auto transform = projection * view * model;
// MVP matrix를 전달
auto transformLoc = glGetUniformLocation(m_program->Get(), "transform");
glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
```

#### 3차원 큐브와 depth test

- transformation을 구현했기 때문에 이제 3차원 도형들을 그릴 수 있다.
- 아래와 같은 긴 배열로 3차원 큐브를 그릴 수 있다. 큐브의 꼭짓점이 8개인데 정점 데이터가 24개나 되는 이유는 각 꼭짓점에서 만나는 3개의 면의 normal vector가 모두 다르기 때문에 같은 꼭짓점에 대한 정점 데이터가 3개씩 있어서 그렇다. (<https://rinthel.github.io/opengl_course>의 7.Transformation의 49번 슬라이드에서 가져옴)

```c++
float vertices[] = {
    // x, y, z, s, t
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,

    -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

    0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
    0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
};

uint32_t indices[] = {
    0,  2,  1,  2,  0,  3,
    4,  5,  6,  6,  7,  4,
    8,  9, 10, 10, 11,  8,
    12, 14, 13, 14, 12, 15,
    16, 17, 18, 18, 19, 16,
    20, 22, 21, 22, 20, 23,
};
```

- glBufferData()에 배열 크기를 전달할 때 VBO는 `sizeof(float) * 120`, EBO는 `sizeof(uint32_t) * 36`을 지정하고 정점 속성으로는 0번에 위치, 2번에 텍스쳐 좌표를 입력한다.
- 그릴 떄는 glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);을 사용한다.
- 이 큐브를 그냥 그리게 되면 박스의 뒷면이 앞면 앞에 그려져 큐브가 이상하게 되는 문제점이 있다. 이유는 depth test를 활성화 하지 않아 렌더링 순서상 나중에 그려지는 면을 이미 그려진 그림 위에 덮어 씌우기 때문이다. 그래서 뒷면이 앞면에 가려 보이지 않게 하기 위해 depth test를 활성화 시켜주어야 한다.
- Context::Render() 함수의 앞에 아래 두 줄을 추가해 주면 된다.

```c++
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// depth test 활성화
glEnable(GL_DEPTH_TEST);
```

#### (+) glfwGetTime()을 이용한 큐브 애니메이션

- MVP matrix를 설정하는 코드를 매 프레임 실행되는 Context::Render()로 옮기고, 큐브의 위치, 회전, 크기를 조절하는 model matrix를 매 프레임마다 수정하게 되면 큐브의 회전과 움직임을 구현할 수 있게 된다.

```c++
void Context::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto projection = glm::perspective(glm::radians(45.0f),
        (float)640 / (float)480, 0.01f, 10.0f);
    auto view = glm::translate(glm::mat4(1.0f),
        glm::vec3(0.0f, 0.0f, -3.0f));
    auto model = glm::rotate(glm::mat4(1.0f),
        glm::radians((float)glfwGetTime() * 120.0f),
        glm::vec3(1.0f, 0.5f, 0.0f));
    auto transform = projection * view * model;
    m_program->SetUniform("transform", transform);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
```

- 그리고 각 큐브마다 위치 벡터를 다르게 설정하고, for 문 안에서 model matrix를 다르게 설정한 뒤 glDrawElements를 호출해 주면 여러개의 큐브를 그릴 수 있다.

```c++
for (size_t i = 0; i < cubePositions.size(); i++){
    auto& pos = cubePositions[i];
    auto model = glm::translate(glm::mat4(1.0f), pos);
    model = glm::rotate(model,
        glm::radians((float)glfwGetTime() * 120.0f + 20.0f * (float)i),
        glm::vec3(1.0f, 0.5f, 0.0f));
    auto transform = projection * view * model;
    m_program->SetUniform("transform", transform);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
```

#### Camera

- 3D 공간을 바라보는 주체. 화면에서 보이는 3D 공간은 카메라의 시선이다.
- view matrix를 사용해서 카메라의 위치와 보고 있는 지점에 따라 3D 공간을 어떻게 화면에 보이게 할 지를 결정한다.
- 카메라 파라미터로는 position, target, up vector가 있다. 각각 카메라의 위치, 카메라가 바라보는 지점의 위치, 카메라 화면의 세로 축 방향을 뜻한다.
- 이 3개의 파라미터를 사용해서 카메라 기준 좌표를 월드 좌표로 바꾼 행렬, 즉 카메라 행렬을 구한 뒤 그것의 역행렬을 구하면 view matrix를 만들어 낼 수 있다.

> 1. 우선 아래 수식을 통해서 카메라를 기준으로 한 x, y, z축을 구한다.
> $e$ 를 카메라의 위치, $t$ 를 카메라가 보는 위치, $u$를 카메라의 up vector라고 할 때,
> $z = \frac{e-t}{\lVert e-t \rVert}, x = \frac{u \times z}{\lVert u \times z \rVert}, y = z \times x, p = e$
> 2. 그 다음 x, y, z, p 벡터를 이용해 카메라 행렬을 구하고, 그 행렬의 역행렬을 구해 view matrix를 구한다.
> $camera = \begin{pmatrix} x & y & z & p \\ 0 & 0 & 0 & 1 \end{pmatrix}$
> $view = \begin{pmatrix} x^T & -x^Tp \\ y^T & -y^Tp \\ z^T & -z^Tp \\ 0 & 1 \end{pmatrix}$

- 코드로는 아래와 같이 구현한다.
- cameraPos, cameraTarget, cameraUp은 glm::vec3의 값으로 가져온다.

```c++
auto cameraZ = glm::normalize(cameraPos - cameraTarget); // 카메라 z방향은 e - t 벡터의 방향이다.
auto cameraX = glm::normalize(glm::cross(cameraUp, cameraZ)); // 카메라 x 방향은 up 벡터와 z방향의 외적이다.
auto cameraY = glm::cross(cameraZ, cameraX); // 카메라 y방향은 z방향과 x방향의 외적이다.
```

- 구한 벡터들로 카메라 행렬(view matrix)을 구한다.

```c++
auto cameraMat = glm::mat4(
    glm::vec4(cameraX, 0.0f),
    glm::vec4(cameraY, 0.0f),
    glm::vec4(cameraZ, 0.0f),
    glm::vec4(cameraPos, 1.0f));

auto view = glm::inverse(cameraMat); // view 행렬은 카메라 행렬의 역행렬이다.
```

- glm에는 저 역할을 대신 해주는 lookAt 함수가 있으므로 이것을 사용하면 된다.

```c++
auto view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
```

#### Interactive Camera (카메라 조작하기)

##### 1. 이동 (키보드)

- 카메라를 키보드와 마우스를 이용해서 위치와 방향을 바꿀 수 있도록 할 것이다.
- 카메라의 위치, front vector, up vector를 저장하는 변수를 context.h에 추가하고, ProcessInput이라는 새로운 함수를 만들어서 키보드 입력을 처리할 수 있게 한다.

```c++
void Context::ProcessInput(GLFWwindow* window) {
    // 카메라 앞/뒤 이동
    const float cameraSpeed = 0.01f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraFront;

    // 카메라 좌우 이동
    // 카메라 오른쪽 방향은 up 벡터와 -front 벡터의 외적이다.
    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront)); 
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraRight;    

    // 카메라 상하 이동
    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;
}
```

- 그리고 cameraTarget 대신 카메라의 위치와, front vector를 더한 것을 lookAt 함수에 넣어준다.

```c++
// 카메라의 위치, 타겟 위치를 이용한 view 행렬 계산
auto view = glm::lookAt(m_cameraPos, m_cameraFront + m_cameraPos, m_cameraUp);
```

- main.cpp의 메인 루프에서 저 함수를 호출해주면 이제 카메라의 위치를 키보드로 움직일 수 있게 된다.

```c++
while (!glfwWindowShouldClose(window)) {
    glfwPollEvents(); // 키보드, 마우스 등 각종 이벤트 수집
    // 키 입력 처리
    context->ProcessInput(window);
    // 렌더링 
    context->Render();
    // 프론트/백 버퍼 교체
    glfwSwapBuffers(window);
}
```

##### 2. 회전 (마우스)

- 마우스를 이용해서 회전을 구현하기 위해서 마우스의 움직임을 받을 수 있도록 하고, 추가적으로 회전 이동을 제어하기 위해 마우스 오른쪽 버튼을 눌렀을 때만 이동할 수 있도록 할 것이다.
- Euler angle은 물체의 회전 정보를 나타내는 대표적인 방식으로, x축을 기준으로 회전한 각도를 pitch, y축 기준 회전각을 yaw, z축 기준 회전각을 roll이라고 한다.
- 카메라 회전에는 pitch와 yaw 수치만을 조절해서 카메라의 front vector의 방향을 구한다.

##### 구현 과정

- context.h에 카메라의 pitch와 yaw를 저장하는 변수와 이전 마우스 포인터 위치, 마우스 오른쪽 키를 눌렀는지 여부를 저장하는 변수를 추가한다.
- 마우스 움직임을 입력받는 MouseMove() 함수와, 마우스 버튼을 입력받는 MouseButton() 함수를 추가한다. (Context 클래스)

```c++
// 마우스 입력(카메라 방향)
void Context::MouseMove(double x, double y) {
    // 마우스 오른쪽 클릭을 유지한 상태에서만 이동
    if (!m_cameraControl)
        return;
    // 현재 위치 구함
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.8f;
    // 카메라 좌우 각
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    // 카메라 상하 값
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    // 카메라 각도의 상한과 하한을 설정
    if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    // 이전 위치 저장
    m_prevMousePos = pos;
}

// 마우스 키 입력
void Context::MouseButton(int button, int action, double x, double y) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            // 마우스 조작 시작 시점에 현재 마우스 커서 위치 저장
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE) {
            m_cameraControl = false;
        }
    }
}
```

- MouseMove() 함수에서는 현재 마우스 포인터 위치와 이전 마우스 포인터 위치의 차이를 구해서 변화한 양만큼 카메라의 회전 각도를 조절한다. MouseButton()에서는 마우스 오른쪽 버튼을 누른 경우에는 m_cameraControl을 true로 지정해 카메라의 회전각을 조절할 준비를 하고 이전 마우스 위치를 m_prevMousePos에 저장한다.
- 마우스 관련 이벤트 함수에는 GLFWwindow를 받지 않고 있기 때문에 main.cpp에서 마우스 입력을 담당하는 콜백함수를 만들어 그 함수에서 위의 함수들을 호출하게 했다.

```c++
// 마우스 이동 이벤트 처리
void OnCursorPos(GLFWwindow* window, double x, double y){
    auto context = (Context*)glfwGetWindowUserPointer(window);
    // 마우스 이동 처리
    context->MouseMove(x, y);
}

// 마우스 클릭 이벤트 처리
void OnMouseButton(GLFWwindow* window, int button, int action, int modifier) {
    auto context = (Context*)glfwGetWindowUserPointer(window);
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    context->MouseButton(button, action, x, y);
}
```

- main() 함수의 반복문 실행 전에 콜백 함수를 등록한다.

```c++
// 마우스가 이동했을 때 실행할 함수
glfwSetCursorPosCallback(window, OnCursorPos);
// 마우스를 클릭했을 때 실행할 함수
glfwSetMouseButtonCallback(window, OnMouseButton);
```

- 이제 실행하면 마우스 오른쪽 버튼을 눌렀을 때 마우스를 움직여서 카메라의 회전각을 바꿀 수 있다.

### depth test 켜고 끄기

```c++
glEnable(GL_DEPTH_TEST); // depth test 킴
glDisable(GL_DEPTH_TEST); // depth test 끔
```

- 기본적으로 OpenGL에서는 depth test가 꺼져 있다.
- 3D 오브젝트를 그릴 때는 depth test를 켜야 하고, Scene의 오브젝트를 모두 그린 후 GUI를 그릴 때는 depth test를 끄고 그린다.(ImGUI는 내부적으로 끄는 코드가 있으므로 우리가 명시할 필요는 없음)

- 추가적인 함수

```c++
glDepthMask(GL_FALSE); // depth buffer가 업데이트 되는 것을 막는다. (GL_TRUE로 하면 업데이트 됨)
glClearDepth(1.0f); // depth buffer의 초기값 설정
```

- depth test의 비교 연산자 설정

```c++
glDepthFunc(GL_LESS);
```

- GL_LESS가 기본값으로, 그려질 픽셀이 해당 위치에 이미 있는 픽셀의 z값보다 작으면(즉, 앞에 있으면) 이미 있는 픽셀을 덮어씌우게 된다.(뒤에 있는 물체를 가린다는 뜻이다.)
- 함수에 넣을 수 있는 인자는 GL_ALWAYS(코드 상 나중에 그려지는 물체가 반드시 앞에 오게 된다.), GL_NEVER(코드 상 먼저 그려지는 물체가 반드시 앞에 오게 된다.), GL_GREATER(위치 상 뒤에 있는 물체가 앞에 그려 진다.) 등이 있다.

### depth buffer 관련 사항

1. 깊이 값의 왜곡
    - perspective projection을 적용하면 z값이 0~1사이로 정규화 될 때 1 / z 함수를 뒤집어 놓은 듯한 형태가 된다. 이렇게 되면 카메라와 가까운 경우에는 깊이 값이 극단적으로 변화하고, 먼 오브젝트끼리는 z값의 차이가 거의 없게 된다.
2. z-fighting
    - 앞에서 언급한 깊이 값의 왜곡 때문에 발생하는 현상이다. 서로 다른 두 면의 좌표값 차이가 거의 없는 경우 그 물체로부터 멀어질 때, 두 면의 깊이 값 차이가 거의 없어져서 근소한 차이로 뒤에 있는 면이 앞에 있는 면과 겹쳐보이는 현상이 발생하는데 이를 z-fighting이라고 한다.
    - 이를 방지하려면, 서로 다른 두 물체의 면을 너무 가깝게 하지 말고, perspective projection에서 near값을 너무 작게 하지 않으며, 더 정확한 depth buffer를 사용해야 한다.

### stencil buffer

- 특정 픽셀에만 그림을 그리도록 설정하기 위해 사용하는 8bit의 정수형 버퍼 - 공판화를 떠올리면 쉽다.
- stencil test는 depth test에 앞서 실행된다.

```c++
glEnable(GL_STENCIL_TEST); // 스텐실 테스트를 활성화
glStencilMask(0xFF); // 스텐실 버퍼에 적용할 마스크값(0~255) 
// 255이면, 각 비트는 스텐실 버퍼에 그대로 작성되고, 0이면 비트는 항상 0으로 쓰여진다.

// 스텐실 버퍼도 업데이트를 위해 초기화 해주어야 한다.
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 
```

### stencil buffer의 함수

```c++
// 스텐실 버퍼
glStencilFunc(GL_EQUAL, 1, 0xFF);
```

- 1번째 인자에 비교 연산자를 써준다.
- 2번째는 각 비트의 스텐실 값과 비교할 값을 써준다.
- 3번째는 비교하기 전 각 비트의 값과 bit AND 연산을 해줄 비트 마스크를 지정해준다.
- 위의 함수는 각 비트의 값이 1이면 스텐실 테스트를 통과해서 해당 비트가 그려진다는 것을 의미한다.

```c++
// 스텐실 테스트와 depth 테스트가 성공 또는 실패했을 때 수행할 행동을 지정
glStencilOp(sfail, dpfail, dppass);
```

- sfail에는 스텐실 테스트가 실패했을 때 수행할 행동, dpfail에는 스텐실 테스트는 성공했으나 depth test가 실패한 경우, dppass에는 둘 다 성공한 경우 수행할 행동이 들어간다.
- 행동에는 GL_KEEP (스텐실 값 유지), GL_ZERO (0으로 만듦), GL_REPLACE (glStencilFunc에서 지정한 2번째 인자의 값으로 바꿈), GL_INVERT (스텐실 값에 bit not 연산 수행) 등이 있다. (<https://heinleinsgame.tistory.com/25> 참고)

### blending

- 그리려고 하는 색상과 프레임버퍼에 이미 입력되어 있는 색상끼리 연산해서 새로운 색상을 만들어 내는 것
- 주로 반투명한 오브젝트를 그리고, 그 오브젝트와 뒤에 있는 오브젝트끼리 만나서 색이 합성되는 효과를 내기 위해 사용된다.
- 블랜딩을 통한 최종 색깔은 아래 공식에 의해 결정된다.
- $\vec{C_{result}}=\vec{C_{source}}*F_{source}+\vec{C_{dest}}*F_{dest}$
- c_source는 그리려고 하는 색상, f_source는 그 색상에 적용하려는 함수
- c_dest는 그 픽셀의 원래 색상, f_dest는 그 색상에 적용하려는 함수이다.
- 대부분의 경우 f_source에는 c_source의 alpha 값을, f_dest에는 (1 - c_source의 alpha)을 주로 넣음

```c++
glEnable(GL_BLEND); // 블랜딩 활성화
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 블랜딩 함수 - f_source와 f_dest를 설정한다.
glBlendEquation(GL_FUNC_ADD) // 위의 식에서 두 부분을 어떤 연산자로 연결할 지를 정한다. (기본 값은 + 연산자)
glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO); // 이 함수를 쓰면 색상에서 color(RGB)와 alpha 값에 각각 다른 함수를 곱할 수 있다.
```

### face culling

- 앞면 또는 뒷면을 그리지 않도록 설정하는 작업을 뜻한다.
- 우리가 큐브를 볼 때, 뒤의 3면은 볼 수 없는데 보이지 않는 3면을 그리지 않으므로써 성능을 향상시키는 데에 쓸 수 있다.
- 삼각형을 이루는 점의 순서에 따라 앞/뒷면이 결정되는데, 주로 시계 반대 방향(CCW) 순서인 면이 앞면이 된다.

```c++
glEnable(GL_CULL_FACE); // face culling 활성화(평소에는 꺼져있다.)
glFrontFace(GL_CCW); // 어떤 방향을 앞면으로 할 것인가?(GL_CCW가 기본)
glCullFace(GL_BACK); // 그리지 않을 면을 설정
```

- 평면과 같이 면이 1개만 있는 물체를 세워 놓을 경우 face culling을 했을 때, 앞에서는 보이지만 뒤에서는 안 보이게 될 수 있다. 이런 경우에는 face culling을 끄고 그려야 한다.

### frame buffer

- color, depth, stencil buffer의 집합체
- GLFW를 통해 context를 생성하면 화면에 그림을 그리기 위한 프레임버퍼가 자동으로 만들어진다.
- 개발자가 직접 프레임버퍼를 생성할 수 있는데 이를 이용하면 렌더링된 장면을 바로 텍스쳐로 활용할 수도 있고, 포스트 프로세싱(post-processing)을 할 수도 있다

#### 사용 과정

1. 프레임 버퍼 객체를 만든다
2. color, depth, stencil 버퍼를 만든다
3. 2에서 만든 렌더 버퍼들을 프레임 버퍼에 연결한다.
4. 해당 렌더 버퍼 조합을 프레임 버퍼로 사용할 수 있는지를 확인한다.(하드웨어마다 특정 조합은 프레임버퍼로 쓸 수 없는 경우가 있다고 한다.)

### CubeMap

- 2D 텍스쳐 6장으로 정육면체 형태로 이어붙인 텍스쳐(주사위를 생각하면 된다.)로 주변 환경을 그릴 때 쓴다.
- 원점을 기준으로 하는 3차원 방향 벡터가 큐브맵의 텍스쳐와 만나는 지점의 픽셀값을 샘플링한다.
- 이 큐브맵을 이용해서 주변의 뒷배경을 그릴 수 있는데 이것이 SkyBox이다. (Skybox texture라고 검색해서 여러가지 예제를 볼 수 있다.)

### OpenGL에서 CubeMap 설정

- 일반적인 텍스쳐처럼 ID를 통해서 접근한다. glGenTextures(1, &m_texture)로 생성, glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture)로 바인딩, glDeleteTextures(1, &m_texture)로 삭제한다.
- 3차원이므로 래핑을 할 때 3개의 차원에 대해 초기화를 해준다.(유형에는 GL_TEXTURE_CUBE_MAP을 사용)

```c++
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
```

- 6개의 이미지를 불러와서 GPU 데이터로 넘겨준다. 이때 각 면의 이미지를 따로 불러와서 glTexImage2D()로 넘겨주어야 하는데, 첫 번째 인자로 GL_TEXTURE_CUBE_MAP_POSITIVE_X부터 1씩 증감하면서 넣어주면 된다.

```c++
glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
            image->GetWidth(), image->GetHeight(), 0,
            format, GL_UNSIGNED_BYTE, image->GetData());
```

### Environment mapping

- SkyBox의 텍스쳐를 특정 오브젝트에 적용하는 방식 - 즉, SkyBox의 모습이 거울처럼 렌더링하고자 하는 오브젝트에 그려지게 된다.
- SkyBox의 모습이 물체에 반사되거나 굴절되어 보이도록 할 수 있다.
- 물체를 바라보는 시선 벡터와, 바라보는 위치의 법선 벡터를 구한 뒤, 그들을 이용해서 반사 벡터를 구한다. 그 후 반사 벡터와 SkyBox가 만난 지점의 픽셀이 그 큐브에 그려지게 된다.

```glsl
vec3 I = normalize(position - cameraPos);
vec3 R = reflect(I, normalize(normal));
fragColor = vec4(texture(skybox, R).rgb, 1.0);
```

- 단독으로 사용할 경우 SkyBox의 모습이 그대로 물체에 그려지므로, 그 물체 주변에 다른 물체들이 있더라도 무시되고 오로지 SkyBox의 모습만 그려져 비현실적일 수 있다. 그래서 빛을 렌더링하는 일반적인 쉐이더들과 함께 사용한다.

### Buffer Update

- VBO에 저장된 버퍼 데이터는 중간에 변경될 수 있다.
- 이를 위해서 VBO를 만든 뒤 glBufferData()에서 4번째 인자에 GL_DYNAMIC_DRAW를 전달하여 버퍼가 바뀜을 OpenGL에 알려야 한다.
- 데이터 수정을 담당하는 함수들

```c++
 // 데이터 일부 복사
 // 바인딩된 버퍼(1번 인자)의 데이터 중 12번째 부터 넣고자 하는 data의 크기만큼 data의 내용을 버퍼에 넣음
glBufferSubData(GL_ARRAY_BUFFER, 12, sizeof(data), &data);

// 해당 버퍼의 포인터를 직접 가져오는 방법
// target 버퍼의 내용을 2번째 인자의 목적으로 void* 형태로 가져옴(이때, 버퍼는 glBindBuffer로 바인딩 되어 있어야 함)
// GL_WRITE_ONLY : 쓰기만 가능, GL_READ_ONLY : 읽기만 가능, GL_READ_WRITE : 읽기&쓰기 모두
void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
// 이렇게 하면 일반적인 메모리 함수를 통해 버퍼 데이터를 바꿀 수 있다.
memcpy(ptr, data, sizeof(data));
// 사용이 끝났으면 unmap을 해서 실수로 변경하는 일이 없도록 함.
glUnmapBuffer(GL_ARRAY_BUFFER);

// 버퍼에서 다른 버퍼로 복사(위의 두 함수보다 성능상 유리)
// 우선 두 버퍼를 바인딩 함
glBindBuffer(GL_COPY_READ_BUFFER, vbo1);
glBindBuffer(GL_COPY_WRITE_BUFFER, vbo2);

// GL_COPY_READ_BUFFER에서 roffset(여기서는 4)번 인덱스 부터 8 * sizeof(float)크기 만큼의 데이터를
// GL_COPY_WRITE_BUFFER의 woffset(여기서는 8)번 인덱스 부터 8 * sizeof(float)크기 만큼의 공간으로 복사
glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 4, 8, 8 * sizeof(float));
```

- (+) 텍스쳐의 이미지 데이터 변경을 위한 glTexSubImage2D()도 있다. : <https://learn.microsoft.com/ko-kr/windows/win32/opengl/gltexsubimage2d>

### 고급 GLSL

#### Built-in variable

- Built-in variable은 shader 내에서 사용할 수 있도록 미리 정의된 변수를 뜻한다.

1. gl_Position
   - 현재 정점의 공간 좌표(clip-space position), vertex shader에서 사용하는 필수 변수
2. gl_Pointsize
   - GL_POINTS를 이용해서 그림을 그리고자 할 때, 점의 크기를 정할 수 있다.
   - vertex shader에서 설정하며, gl_PointSize = size;의 형식이다.
   - glEnable(GL_PROGRAM_POINT_SIZE)으로 활성화 한 뒤 사용 가능하다.

    ```glsl
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        gl_PointSize = gl_Position.z;
    }
    ```

3. gl_VertexID
   - 현재 vertex shader에서 처리 중인 정점의 인덱스
4. gl_FragCoord
   - fragment shader에서 사용할 수 있는 현재 픽셀의 화면상 위치값
   - 물체의 위치와 무관하므로 화면에서 고정된 위치에 특정한 이미지를 그릴 때(배경 같은거) 효과적이다.

    ```glsl
    void main() {
        if(gl_FragCoord.y < 300)
            FragColor = vec4(1.0, 0.5, 0.0, 1.0);
        else
            FragColor = vec4(1.0, 1.0, 0.0, 1.0);
    }
    ```

5. gl_FrontFacing
   - fragment shader에서 사용할 수 있으며, 현재 픽셀이 앞면인지 뒷면인지를 저장하는 bool 값
   - 앞/뒷면에 서로 다른 텍스쳐를 입히려고 할 때 유용하다.
   - face culling을 꺼야 의미가 있다.

    ```glsl
    uniform sampler2D frontTexture;
    uniform sampler2D backTexture;

    void main() {
        if(gl_FrontFacing)
            FragColor = texture(frontTexture, TexCoords);
        else
            FragColor = texture(backTexture, TexCoords);
    }
    ```

6. gl_FragDepth
   - fragment shader에서 사용할 수 있으며, 현재 픽셀의 깊이 값을 저장한다.
   - 기본 값으로는 gl_FragCoord.z가 들어가 있다.
   - 이 값을 이용해서 depth를 변경할 수 있으나, 이렇게 되면 early depth test가 비활성화 된다.

- 더 많은 Built-in variables : <https://www.khronos.org/opengl/wiki/Built-in_Variable_(GLSL)>

#### Interface block

- vertex shader와 fragment shader 간의 link를 성공시키려면 두 변수간의 in/out 변수 연결이 잘 되어야 한다.
- in/out 변수가 많을 수록 이들 간의 관리가 어려워지므로, 이들의 관리를 위해 구조체처럼 하나로 묶어 놓은 것을 Interface block이라고 한다.
- vertex shader에서는 out BLOCK_NAME {}, fragment shader에서는 in BLOCK_NAME {}의 형태로 쓴다.
- 구조체와 마찬가지로 내부 변수에 접근할 때는 . 연산자를 사용한다.

- vertex shader : out VS_OUT {...} vs_out;이 인터페이스 블록이다.
- main에서 값을 지정할 때, vs_out.TexCoords 같이 . 연산자를 사용해서 내부 변수에 접근한다.

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
 
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// out 변수를 관리하는 interface block
out VS_OUT {
    vec2 TexCoords;
} vs_out;
 
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vs_out.TexCoords = aTexCoords;
}
```

- fragment shader : in VS_OUT {...} fs_in;이 인터페이스 블록이다.
- 블록 이름 VS_OUT은 vertex shader에 있는 것과 같아야 하지만, 변수 이름은 달라도 상관없다.

```glsl
#version 330 core
 
out vec4 FragColor;

// in 변수를 관리하는 interface block
in VS_OUT {
    vec2 TexCoords;
} fs_in;
uniform sampler2D texture;
 
void main() {
    FragColor = texture(texture, fs_in.TexCoords);
}
```

#### Uniform Buffer Object (UBO)

- 여러 shader들이 공통으로 사용할 수 있는 uniform 변수들을 저장하는 버퍼 오브젝트이다.
- VBO, EBO와 같은 다른 버퍼들처럼 glGenBuffers로 생성하고, glBindBuffer()로 바인딩해서 쓴다.(GL_UNIFORM_BUFFER가 타입)
- shader에서 layout (std140) uniform 변수이름 {...};의 형식으로 작성한다.

```glsl
layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};
```

- std140 메모리 레이아웃에서는 GPU 메모리 접근성의 효율을 높이기 위해 변수들을 4 또는 16바이트 크기로 정렬한다.
- int, bool, float 같은 스칼라는 4바이트, vec2는 8바이트, vec3, vec4는 16바이트, 스칼라 또는 벡터로 이루어진 배열은 요소 1개당 16바이트, matN은 16 * N바이트, 구조체는 안의 요소들의 크기 합과 같으나 16바이트의 배수가 되도록 패딩한다.
- 레이아웃 예시 (<https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL> 에서 가져옴)

```glsl
layout (std140) uniform ExampleBlock
{
                     // base alignment  // aligned offset
    float value;     // 4               // 0 
    vec3 vector;     // 16              // 16  (offset must be multiple of 16 so 4->16)
    mat4 matrix;     // 16              // 32  (column 0)
                     // 16              // 48  (column 1)
                     // 16              // 64  (column 2)
                     // 16              // 80  (column 3)
    float values[3]; // 16              // 96  (values[0])
                     // 16              // 112 (values[1])
                     // 16              // 128 (values[2])
    bool boolean;    // 4               // 144
    int integer;     // 4               // 148
}; 
```

- float value 4바이트 + padding 12바이트 + vec3 vector 16바이트 + mat4 matrix 64바이트 + float values[3] 16*3=48바이트(float 4바이트에 12바이트 패딩이 붙음) + bool boolean 4바이트 + int integer 4바이트로 구성되어 있다.
- 그래서 최종적으로 위의 데이터의 총 크기는 152바이트가 된다.

#### UBO 사용

- 다른 버퍼와 비슷한 방식으로 생성, 바인딩, 데이터 초기화를 한다.
- target으로는 GL_UNIFORM_BUFFER를 넣고, glBufferData의 2번째 부분에는 쉐이더에서 만든 uniform 블록의 크기를 쓴다.

```c++
unsigned int uboExampleBlock;
glGenBuffers(1, &uboExampleBlock);
glBindBuffer(GL_UNIFORM_BUFFER, uboExampleBlock);
glBufferData(GL_UNIFORM_BUFFER, 152, NULL, GL_STATIC_DRAW); // 152 bytes
glBindBuffer(GL_UNIFORM_BUFFER, 0);
```

- glGetUniformBlockIndex(programID, blockName)으로 programID번 shader 프로그램의 uniform block의 인덱스를 가져옴
- glUniformBlockBinding()을 이용해 바인딩 된 UBO와 프로그램의 uniform 블록을 연결한다.
- glBindBufferBase()로 UBO의 번호를 지정해 준다.

```c++
auto lightIndex = glGetUniformBlockIndex(programId, "Lights"); // Lights라는 이름의 uniform block을 찾는다.
glUniformBlockBinding(programId, lightIndex, 2); // Lights와 2번 UBO를 연결한다.
glBindBufferBase(GL_UNIFORM_BUFFER, 2, uboExampleBlock); // uboExampleBlock이 2번 UBO가 된다.
```

#### Geometry Shader

- vertex shader와 fragment shader 사이에 있는 shader
- 하나의 primitive (GL_POINT, GL_TRIANGLE 같은 거)를 받아 다른 primitive를 출력하는 역할을 한다.

```glsl
#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 2) out;
  
void main() {    
    gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0); 
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.1, 0.0, 0.0, 0.0);
    EmitVertex();
    
    EndPrimitive();
} 
```

- 입력받고자 하는 primitive를 layout (...) in에 정의하고, 출력하고자 하는 primitive는 layout (...) out에 정의
- EmitVertex()로 gl_Position에 정의된 정점을 만들어 냄
- EndPrimitive()로 지금까지 생성한 정점들을 묶어낸 새로운 primitive를 만들어낸다.

### Instancing

- 동일한 오브젝트를 아주 많이 그릴 때 일일이 glDrawArrays()를 호출하면 CPU와 GPU 사이의 통신이 많아져 성능 저하가 발생한다.
- 그래서 동일한 여러 개의 오브젝트를 한 번의 draw call만을 호출해 모두 그리는 것을 Instancing이라고 한다.
- glDrawArraysInstanced(), glDrawElementsInstanced() 함수에 개수를 파라미터로 전달하여 instancing을 할 수 있다.
- gl_InstanceID는 vertex shader에서 쓸 수 있는 built-in variable로 서로 다른 instance를 구분 짓는데 쓴다. 이를 이용해서 각기 다른 오브젝트마다 위치를 달리하거나 다른 특징을 달리하는 등의 차이를 만들어 낼 수 있다.

- VBO를 이용한 Instancing 예시 (<https://learnopengl.com/Advanced-OpenGL/Instancing>에서 가져옴)

```c++
// instance들의 각각의 정보를 가진 VBO를 생성하고 바인딩 한다.
// 이는 우리가 원래 만든 물체의 정점의 정보를 담은 VBO와는 다른 것이다.
unsigned int instanceVBO;
glGenBuffers(1, &instanceVBO);
glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
glBindBuffer(GL_ARRAY_BUFFER, 0);

// vertex attribute 0, 1 setting ...

// 2번 속성에 위에서 만든 VBO를 지정한다.
// 어렇게 되면 물체의 정점 데이터를 담은 VBO와 instanceVBO, 이렇게 2개의 VBO가 바인딩 된다.
glEnableVertexAttribArray(2);
glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
// 각 인스턴스 별로 들어갈 값을 설정
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
glBindBuffer(GL_ARRAY_BUFFER, 0);
// 2번 속성(1번째 인자)을 1개(2번째 인자)의 인스턴스가 나올때마다 업데이트 한다.
// 이 경우 2번 속성은 각 인스턴스 별로 다르게 설정하는 것이 된다. (2번 속성은 gl_InstanceID가 1씩 달라질 때마다 업데이트 된다.)
glVertexAttribDivisor(2, 1);
```

- vertex shader에서는 layout (location = 2) in vec2 aOffset; 부분을 추가해 준다.

### Anti-aliasing

- 앨리어싱 현상이란 렌더링된 오브젝트의 경계 부분에 도트로 된 계단이 보이는 현상을 말한다.
- 이 현상이 발생하는 이유는 rasterization 과정에서 어떤 픽셀 부분을 채울 것인지 말 것인지 2가지 경우만 고려하기 때문이다.
- 그래서 이러한 계단 현상을 없애기 위한 방법들이 Anti-aliasing이다.

#### Anti-aliasing 기법

- SSAA : 원래 해상도보다 더 큰 해상도로 렌더링한 다음 이를 다운 샘플링(이웃하는 픽셀들의 색상 평균을 낸다.)해서 계단 현상을 없애는 기법이다. 큰 해상도로 렌더링할 경우에는 비용이 매우 커지게 된다.(2배 커지면, 4배의 비용이 든다.)
- MSAA : OpenGL에서 기본적으로 제공하는 기법이다. 한 픽셀에 여러 개의 기준 위치를 정하고 그 기준 위치들이 삼각형 안에 몇 개 속하는지 각 픽셀마다 조사한다. 이렇게 해서 전체 기준 위치 중에서 삼각형에 몇 개의 위치가 들어갔는지에 따라 알파 값을 조절해서 계단 현상을 제거한다.

#### OpenGL에서 사용하기

```c++
// glfw에 MSAA를 사용할 것을 미리 알린다. - 다른 종류의 프레임버퍼가 필요하기 때문이다.
// 아래 경우에는 픽셀 하나 당 기준 위치를 4개로 한다.
glfwWindowHint(GLFW_SAMPLES, 4);

// 코드에서 glEnable을 통해 MSAA를 활성화한다.
glEnable(GL_MULTISAMPLE);
```

- 우리가 만든 프레임버퍼는 멀티 샘플이 아니므로 MSAA의 효과를 볼 수 없어서 주석 처리를 해야 한다.
- 만약 직접 만든 프레임버퍼에 MSAA를 적용하고 싶다면 target을 GL_TEXTURE_2D_MULTISAMPLE로 지정해야 하고 다른 함수로 초기화 해준다.

```c++
// creating texture for render target
glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA, width, height, GL_TRUE);
glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
// attaching texture to framebuffer
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
GL_TEXTURE_2D_MULTISAMPLE, tex, 0);
// 렌더 버퍼도 다른 함수를 사용
glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
```

- 쉐이더의 sampler2D 타입은 GL_TEXTURE_2D_MULTISAMPLE을 받을 수 없기 때문에 이를 GL_TEXTURE_2D로 변환하는 작업이 필요하다. 그 작업 과정은 이 사이트를 참고 : <https://learnopengl.com/Advanced-OpenGL/Anti-Aliasing>

### Blinn-Phong Shading

- 기존의 Phong shading은 specular shininess 값이 작은 경우 highlight 부분이 잘려서 빛이 도중에 끊어지는 느낌이 있다.
- 그 이유는 specular를 계산할 때, view와 reflection 간의 각도가 90도 보다 커지면, dot product 값이 0보다 작아져 cutoff가 생기고 빛이 끊기는 느낌이 생긴다.
- 그래서 Blinn이란 사람이 view와 light를 이등분하는 halfway 벡터와 normal 벡터 간의 사잇각으로 계산할 것을 제안했는데, 이를 Blinn-Phong shading이라고 한다.

```glsl
vec3 specColor = texture2D(material.specular, texCoord).xyz;
float spec = 0.0f;
vec3 viewDir = normalize(viewPos - position);
vec3 halfDir = normalize(lightDir + viewDir);
spec = pow(max(dot(halfDir, pixelNorm), 0.0), material.shininess);
```

### Shadow Map

- 3D 렌더링에서 그림자는 입체감을 높여주는 아주 중요한 요소이다.
- 그림자를 렌더링하는 방법에는 여러 가지가 있는데, 그중에서 구현 난이도가 상대적으로 쉽고 성능을 크게 잡아먹지 않아 가장 많이 쓰는 방식이 Shadow map이다.
- 빛의 관점에서 빛이 볼 수 있는 부분은 그림자가 지지 않을 것이고, 빛이 볼 수 없는 부분은 빛이 닿지 못해 그림자가 지게 될 것이다.
- 이를 이용해서 빛의 관점, 즉 빛의 view, projection matrix를 이용해서 물체들의 depth map을 렌더링 한 것이 shadow map이 된다.

#### Shadow map을 그리기

- Shadow map도 일종의 텍스쳐이지만, 일반적인 텍스쳐는 픽셀 타입으로 GL_UNSIGNED_BYTE를 사용하지만, shadow map은 깊이 값을 저장해야 하므로 GL_FLOAT를 사용한다.
- Shadow map을 렌더링 하기 위한 프레임 버퍼도 추가로 필요하다. 이 프레임 버퍼는 depth 값만 필요로 하므로, glFramebufferTexture2D() 함수에 GL_COLOR_ATTACHMENT0대신 GL_DEPTH_ATTACHMENT를 전달한다.
- 그리고, color attachment가 없음을 명시해야 하므로 glDrawBuffer(GL_NONE), glReadBuffer(GL_NONE)를 이용한다.

```c++
// 쉐도우 맵을 위한 프레임 버퍼
glGenFramebuffers(1, &m_framebuffer);
Bind();

m_shadowMap = Texture::Create(width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
m_shadowMap->SetFilter(GL_NEAREST, GL_NEAREST);
m_shadowMap->SetWrap(GL_REPEAT, GL_REPEAT);

glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap->Get(), 0);
glDrawBuffer(GL_NONE);
glReadBuffer(GL_NONE);
```

#### 그림자 직접 적용하기

- 그림자를 그리기 위해서는 기존의 Blinn-Phong shading을 위해 사용된 코드를 변형한 새로운 쉐이더를 사용한다.
- vertex shader에서는 빛의 translation을 저장하기 위한 mat4 uniform 값이 필요하다.
- fragment shader에서는 쉐도우 맵을 받기 위한 sampler2D를 하나 더 만들어주고, Blinn-Phong shading을 통해 픽셀의 색상을 계산한 뒤, 추가적으로 그림자의 세기를 쉐도우 맵으로부터 받아오고, 그것을 픽셀과 곱해서 그림자를 구현해 준다.
- 그림자 세기를 계산하는 함수에서는 빛의 위치를 기준으로 그릴 픽셀의 위치를 계산하고 shadowMap에서 해당 픽셀의 depth 값을 가져온다. 이때 저장된 depth 값이 빛의 위치를 기준으로 계산한 depth보다 작은 경우 그림자가 진 것으로 판단한다.

```glsl
float ShadowCalculation(vec4 fragPosLight) {
    // perform perspective divide
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light’s perspective (using
    // [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light’s perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
    return shadow;
}

// 거기에 추가적으로 그림자 세기를 계산해 준다.
// 0이면 그림자가 없는 것이고, 1이면 완전히 그림자가 져서 검게 보인다.
float shadow = ShadowCalculation(fs_in.fragPosLight);

result += (diffuse + specular) * intensity * (1.0 - shadow);
```

- 그러나 이렇게 만든 후 실행을 하게 되면(12-3-1의 코드를 실행해 볼 것), 그림자가 지지 않아야 되는 부분에 검은색 줄무늬가 생기는데 이는 shadow acne라고 하는 현상이다.
- 이 현상이 발생하게 되는 원인은 쉐도우 맵의 크기가 한정적이기 때문이다. 그래서 광원으로부터 거리가 먼 픽셀들은 동일한 depth값을 가질 가능성이 높아지게 되고 이렇게 정확하지 않은 depth값으로 인해 줄무늬 형상이 나타나게 되는 것이다.
- 이를 해결하기 위해 closestDepth 값에 작은 편향치(bias)를 더한 것을 currentDepth와 비교하는 방식을 사용할 수 있다. (코드에서는 bias를 이항했다.)

```glsl
float bias = 0.005;
float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
```

- 이 두 줄을 추가하면 대부분의 경우에서 shadow acne이 사라지게 된다. 그러나 빛의 방향과 표면의 법선 벡터가 크게 차이나는 경우(빛이 표면에 거의 수평하게 입사되는 경우)에는 여전히 shadow acne이 남아있게 된다. 이를 해결하기 위해서는 빛의 방향과 표면 법선 벡터의 차이가 크면 bias도 크게 주는 방식을 이용할 수 있다.

```glsl
// vec3 normal, vec3 lightDir를 인자에 추가한다.
float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005); // bias는 0.005 ~ 0.05 사이의 가변값이 됨

// main 함수에서의 호출 부분
float shadow = ShadowCalculation(fs_in.fragPosLight, pixelNorm, lightDir);
```

- 이렇게 하면 빛의 방향과 면의 각도가 차이가 크더라도 조정된 bias에 의해 shadow acne는 나타나지 않게 된다. 안타깝게도 커진 bias는 새로운 문제를 야기한다.
- bias가 클 때, 그림자가 물체의 위치와 약간 떨어져서 그려지게 되는데 이 현상을 Peter panning이라고 한다.
- 이를 막기 위해서는 bias의 범위를 잘 조절해서 너무 커지지 않게 하거나, shadow map을 그릴 때 face culling을 해서 뒷면만 그리게 하도록 하면 된다. (강의에서는 첫 번째 방법으로, bias를 0.001~0.02 범위로 조절 했다.)

```glsl
float bias = max(0.02 * (1.0 - dot(normal, lightDir)), 0.001); // bias는 0.001 ~ 0.02 사이의 가변값이 됨
```

#### Directional light에서의 쉐도우 맵

- Directional light의 경우에는 light projection을 orthogonal로 바꾼다. 그리고 attenuation은 항상 1.0으로 두며, 모든 방향에서 똑같아야 하므로 light.position 속성은 사용하지 않고, light.direction만 사용한다.

```glsl
vec3 result = ambient;
vec3 lightDir;
float intensity = 1.0;
float attenuation = 1.0;
// Directional light - struct Light에 int directional을 추가함
if (light.directional == 1) {
    lightDir = normalize(-light.direction);
}
```

#### Oversampling

- 위의 코드에서 바닥의 크기를 10 x 10에서 40 x 40으로 확장한 뒤에 directional light를 켜게 되면 directional light여서 바닥에 그림자가 생기면 안되지만, 바닥 끝 부분에 그림자가 있는 것을 확인할 수 있고, 다른 쪽에는 큐브가 없는 곳임에도 큐브의 그림자가 렌더링 된다. 이러한 비정상적인 그림자를 Oversampling이라고 한다.
- 원인은 쉐도우 맵을 위한 텍스쳐를 지정할 때 wrapping 방식으로 GL_REPEAT를 사용하고 있어서 범위를 벗어난 경우 원래 쉐도우 맵을 반복해서 적용시키기 때문이다. 해결을 위해서는 GL_CLAMP_TO_BORDER를 사용하고 Border의 색을 지정해 주면 된다. 색상을 흰색으로 지정해주면, 쉐도우 맵의 범위를 벗어난 영역에는 그림자가 생기지 않아서 비정상적인 그림자가 렌더링되지 않는다.

```c++
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::vec4(1.0f));
```

#### Jagged edge와 PCF

- 12-3-2의 코드를 실행한 뒤 빛을 움직이다보면 만들어진 그림자가 부드럽지 않고 모서리가 계단 형태로 딱딱한 모양을 보이는 경우가 있는데 이를 Jagged edge라고 한다. 이 현상은 depth map의 해상도가 제한되어 있어서 나타난 문제이다.
- 조금 더 부드러운 가장자리의 그림자를 만들기 위해 사용하는 기법이 PCF(Percentage Closer Filtering)이다.
- depth map으로 부터 하나가 아닌 여러 개의 depth값을 수집한 뒤 수집한 depth들로 부터 계산된 shadow 값의 평균을 구한다.
- 아래 코드에서는 기준 픽셀과 그 주위의 상하좌우 대각선 8개의 인접한 픽셀의 쉐도우 값을 구한 뒤 9개의 평균을 내고 있다. (fragment shader의 ShadowCalculation() 안에 넣어줌)
- 이렇게 하면 가장자리 픽셀의 그림자가 조금 더 부드러워지는 효과가 있다.

```glsl
float shadow = 0.0;
vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
for(int x = -1; x <= 1; ++x) {
    for (int y = -1; y <= 1; ++y) {
        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
}
shadow /= 9.0;
```

#### (+고급) point light에서의 그림자

- point light에서는 light를 기준으로 모든 방향에서 빛이 뻗어나가게 된다. 이렇게 되면 하나의 shadow map으로는 부족한데, 하나의 shadow map은 하나의 방향만 고려할 수 있기 때문이다.
- 이때는 Omni-directional shadow map를 사용해서, 단일 shadow map이 아니라 6개의 shadow map이 모여있는 depth cube map을 생성하고, 쉐이더 맵에서는 samplerCube 타입으로 이를 받아와서 그림자를 계산해 준다.
- 자세한 내용 : <https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows>

### Normal map

- 현재 우리가 만든 물체를 빛으로 비추게 되면 텍스쳐 표면의 요철 모양대로 빛이 비추어지는 것이 아니라, 매끄러운 면에다가 스티커를 붙인 형태로 빛이 비춰 지고 있다.
- 만약 물체의 디테일을 표현하고 싶다면 텍스쳐를 이루는 픽셀 하나하나마다 법선 벡터를 지정해 주면 되는데, 각 픽셀마다의 법선 벡터의 값을 저장하고 있는 것이 바로 normal map이 된다. normal map을 적용하면 vertex를 늘리지 않고도 높은 수준의 geometric detail을 표현할 수 있다.
![normal_map0](/image/brickwall_normal.jpg)
- 위의 사진이 normal map의 예시를 보여주는 사진이다. (source : <https://learnopengl.com/Advanced-Lighting/Normal-Mapping>) 대부분의 픽셀이 연한 파랑색임을 알 수 있는데, 이는 z축 방향으로 나오는 벡터 (0,0,1)를 표현한 것이다. 그리고 물체의 디테일한 부분에서는 법선 벡터가 달라야 하므로 다양한 색상으로 나타나 있는 것을 확인할 수 있다.
- 이제 실제 코드에서는 어떻게 사용되어야 하는지를 보자.

```glsl
// 기존의 lighting 쉐이더에서는 하나의 vec3의 벡터를 받아왔지만, 여기서는 sampler2D 형태로 받아온다.
uniform sampler2D normalMap;

// main 함수 내부
// 원래 텍스쳐에서 색을 들고옴
vec3 texColor = texture(diffuse, texCoord).xyz;
// normal map에서 값을 들고온 뒤, 범위를 [-1, 1]로 조정함
vec3 pixelNorm = normalize(texture(normalMap, texCoord).xyz * 2.0 - 1.0);

// 그리고 이렇게 구한 pixelNorm을 분산광과 반사광을 구하는데에 사용한다.
// Note : 코드의 간소화를 위해서 원래 diffuse, reflect를 계산하는 식을 매우 단순화 했다.
// diffuse 계산
vec3 lightDir = normalize(lightPos - position);
float diff = max(dot(pixelNorm, lightDir), 0.0);
vec3 diffuse = diff * texColor * 0.8;

// reflect 계산
vec3 viewDir = normalize(viewPos - position);
vec3 reflectDir = reflect(-lightDir, pixelNorm);
float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
vec3 specular = spec * vec3(0.5);
```

![normal_map1](/note_image/normal1.png)

#### Tangent space

- 현재 코드에서 평면을 90도 회전시키면 빛이 평면에 이상하게 렌더링되는 것을 확인할 수 있다.(빛이 위에 있으므로 평면 전체가 밝아져야 하지만 반은 어둡고, 반은 밝다.)
![normal_map2](/note_image/normal2.png)
- 이렇게 되는 이유는 normal map으로 부터 얻어낸 normal 값을 world space 기준으로 처리하고 있기 때문이다.
- 이를 해결하기 위해 local space를 기준으로 처리하는 방식을 고려할 수 있지만, 이렇게 되면 normal map을 적용할 면이 기준 평면에 정렬되어 있어야 하므로 입체인 경우에는 쓸 수 없다.
- 그래서 사용할 수 있는 방식이 tangent space를 이용하는 것이다.
- tangent space는 법선 벡터에 수직인 평면이다.
![normal_map3-1](/note_image/normal3-1.jpg)
- tangent space에서 normal vector가 +z 방향임을 이용해서 이를 평면의 표면에 맞게 변환하는 방식을 통해서 어느 각도에서도 normal map이 정상적으로 적용되도록 할 수 있다.
- 그러나 normal vector에 수직인 tangent vector는 무수히 많다. 그렇다면 기준이 되는 tangent vector를 어떻게 구할 수 있을까?
- 그건 바로 아래 그림처럼 fragment를 이루는 삼각형의 세점의 텍스쳐 좌표를 이용하는 것이다.
![normal_map3-2](/note_image/normal3-2.jpg)
- 위의 그림의 식은 아래와 같이 쓸 수 있다. (<https://learnopengl.com/Advanced-Lighting/Normal-Mapping>)
![normal_map4-1](/note_image/normal4-1.png)
- 이를 행렬 계산식으로 바꾼 뒤 역행렬을 곱하면 아래와 같이 된다.
![normal_map4-2](/note_image/normal4-2.png)
- 이렇게 하면 접선 벡터를 구할 수 있고, 이를 normal map의 법선 벡터와 cross product(외적)을 해서 binormal vector를 구해 tangent space를 만들어 낼 수 있다.
- 코드에서는 vertex의 속성으로 tangent 벡터를 추가하고, tangent 벡터를 위의 수식에 구하는 함수를 만들었다.(mesh.h, mesh.cpp 참고)
- 그리고 vertex 쉐이더에서는 tangent와 normal vector를 받아서 계산해주는 부분을 추가한다. (normal.vert)

```glsl
layout (location = 1) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

// main 함수 내부
normal = (invTransModelTransform * vec4(aNormal, 0.0)).xyz;
tangent = (invTransModelTransform * vec4(aTangent, 0.0)).xyz;
```

- fragment 쉐이더에서는 받은 데이터를 토대로 tangent, normal, binormal 벡터를 구한뒤, 이들로 구성된 행렬을 normal map의 벡터와 곱해서 월드 좌표 기준의 normal vector 방향을 구한다. (normal.frag)

```glsl
// 원래 텍스쳐에서 색을 들고옴
vec3 texColor = texture(diffuse, texCoord).xyz;
// normal map에서 값을 들고온 뒤, 범위를 [-1, 1]로 조정함
vec3 texNorm = texture(normalMap, texCoord).xyz * 2.0 - 1.0;
// normal, tangent, binormal vector를 구함
vec3 N = normalize(normal);
vec3 T = normalize(tangent);
vec3 B = cross(N, T);
// TBN matrix
mat3 TBN = mat3(T, B, N);
// 원래 normal에다가 TBN matrix를 곱해서 월드 좌표 에서의 normal vector를 구함
vec3 pixelNorm = normalize(TBN * texNorm);
```

- 이를 구현해서 실행하면 아래 사진 처럼 평면이 누워 있어도 normal map에 의한 효과가 잘 나타남을 알 수 있다.
![normal_map5](/note_image/normal5.png)

#### (+고급) Parallex Mapping

- normal mapping은 텍스쳐 맵에 현실감을 더해주는 아주 중요한 요소이다. 하지만 normal mapping만으로는 더 큰 깊이감을 만들어내기는 힘들다. 그리고 경계면에서는 입체감을 만들어 내기 힘들다는 단점이 있다.
- 그래서 여기에 height map이라는 추가적인 정보를 전달하고, height map을 이용해 실제 바라보는 위치를 계산해 준다면 깊이감 있는 렌더링이 가능하다.
- 자세한 내용 : <https://learnopengl.com/Advanced-Lighting/Parallax-Mapping>

> Note : HDR과 Bloom 부분은 원 강의에서 예제 작성 없이 learnopengl.com의 코드만 설명했으므로 여기서도 소스 코드 작성은 없이 여기에 learnopengl.com의 내용을 가져왔다. 소스 코드는 제목에 있는 사이트에서 가져왔다.

### HDR (High dynamic range) <https://learnopengl.com/Advanced-Lighting/HDR>

- 기본적으로 RGBA 색상값은 0.0~1.0 사이의 값으로 들어온다. 만약 1.0보다 더 큰 값이 fragment shader에 들어오게 된다면 fragment shader는 자동으로 이 값을 1.0으로 만들어준다.
- 그러나 현실에서 같은 하얀색이라고 밝기가 모두 같은 것은 아니다. 흰색 용지와 흰색 형광등을 쳐다볼 때 느낌이 다른 것처럼 빛이 강해서 더 밝은 색으로 표현해야 하는 곳이 있을 것이다. 그러나 모니터는 0.0 ~ 1.0 사이의 밝기 값만 렌더링 할 수 있으므로 이런 차이를 만들어 낼 수 없다.
- 그래서 fragment shader 쉐이더에서 1.0이 넘어가는 색상값을 받은 뒤, 최종적으로 픽셀의 색을 지정해 줄 때는 공식에 의해 다시 0.0 ~ 1.0 범위로 조절해 줄 수 있다. 이처럼 모니터에서 표현 가능한 범위를 벗어난 색상값을 저장하고 사용하는 기법을 HDR이라고 한다.
- HDR을 사용하려면 1.0보다 더 큰 색상값을 저장해야 하므로, 텍스쳐를 만들 때 GL_RGBA가 아닌 GL_RGBA16F나 GL_RGBA32F를 사용해야 한다. 이를 floating point framebuffer라고 한다.

```c++
glBindTexture(GL_TEXTURE_2D, colorBuffer);
// GL_RGBA16F 형식으로 지정해준다.
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
```

#### HDR 기법

##### 1. Tone mapping

- floating point framebuffer에 그려진 texture를 0.0 ~ 1.0 범위에 맞춰서 다시 화면에 그리는 것이다.
- Reinhard tone mapping 방식을 사용해서 모든 색상 값을 다시 0 ~ 1로 조절한다.
- vec3 mapped = hdrColor / (hdrColor + vec3(1.0))

```glsl
void main() {
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    // reinhard tone mapping
    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    // gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));
}
```

##### 2. Exposure tone mapping

- 위처럼 공식을 이용해서 값을 0.0 ~ 1.0 사이로 조절하는 것은 비슷하지만, 노출값을 파라미터로 사용한다는 것과 다른 공식을 사용한다는 차이가 있다.
- vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure)
- 노출값이 클수록 전체적으로 더 밝게 보이고, 어두운 곳의 디테일을 볼 수 있으며, 노출값이 작을 수록 전체적으로 어둡게 보이며 밝은 곳의 디테일을 볼 수 있다.

```glsl
uniform float exposure;
void main() {
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // gamma correction mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(mapped, 1.0);
}
```

### Bloom (<https://learnopengl.com/Advanced-Lighting/Bloom>)

- 1 이상의 밝은 빛을 시각적으로 표현해주는 방법으로 glow effect를 표현하는 후처리 효과의 일종이다.
- 1보다 큰 부분에 밝게 빛나는 효과를 준다.
- 구현 방식
  - 텍스쳐에서 색상 값을 일반적인 방식으로 가져와 빛 계산을 하고 픽셀 값을 FragColor에 저장한다.
  - 이제 텍스쳐에서 색상 값이 1.0이 넘는 부분만 가져와 따로 렌더링한 텍스쳐를 만든다.
  - 바로 전 과정에서 만든 텍스쳐에 blur 연산을 해서 흐릿하게 만든다.
  - 맨 처음 만든 텍스쳐와 blur 연산을 한 텍스쳐를 동시에 그려낸다.
- Multiple render target을 이용해서, 프레임 버퍼에 여러개의 color attachment를 설정할 수 있다.

```glsl
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
```

- 그리고 fragment shader에서 FragColor와 BrightColor를 모두 지정해 준다.

```glsl
void main() {
    // 중략
    // first do normal lighting calculations and output results
    FragColor = vec4(lighting, 1.0);
    // if fragment output is higher than threshold, output brightness color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
```

- c++ 코드에서 두 color attachment를 하나의 프레임 버퍼에 설정하기 위해서는 생성된 두 텍스쳐를 GL_COLOR_ATTACHMENT0과 GL_COLOR_ATTACHMENT1로 붙인 뒤, glDrawBuffers() 함수를 이용해서 그린다.

```c++
// set up floating point framebuffer to render scene to
unsigned int hdrFBO;
glGenFramebuffers(1, &hdrFBO);
glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

unsigned int colorBuffers[2];
glGenTextures(2, colorBuffers);
for (unsigned int i = 0; i < 2; i++) {
    glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
        SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
        GL_TEXTURE_2D, colorBuffers[i], 0);
}
// 두 color attachment를 배열에 넣고 glDrawBuffers에 전달한다.
unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
glDrawBuffers(2, attachments);
```

- blur 연산으로는 Gaussian blur의 변형인 Separate Gaussian blur연산을 쓴다.
- Gaussian blur에서는 모든 픽셀들에 대해 이웃하는 픽셀들의 색상의 가중치를 Gaussian 함수를 바탕으로 계산한 뒤, 계산된 가중치와 픽셀의 색을 곱한 뒤 그들의 평균을 내는 식으로 해당 픽셀의 색을 계산한다.
- 이 계산은 n x n 크기로 이웃 픽셀들에 대해 계산할 때 2차 시간이지만, Gaussian 함수가 대칭형 함수라는 것을 이용해서, 2n번의 계산안에 이를 계산해 낼 수 있다. 이 방식이 Separate Gaussian blur이다.
- 아래는 9 x 9 크기의 이웃 픽셀에 Separate Gaussian blur 연산을 수행하는 코드이다.

```glsl
#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D image;
uniform bool horizontal;

uniform float weight[5] = float[] (
    0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216
);
void main() {
    vec2 tex_offset = 1.0 / textureSize(image, 0); // size of single texel
    vec3 result = texture(image, TexCoords).rgb * weight[0]; // this fragment
    if(horizontal) {
        for(int i = 1; i < 5; ++i) {
        result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    } else {
        for(int i = 1; i < 5; ++i) {
        result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
```

- Separate Gaussian filter를 2개의 frame buffer에 번갈아가며 렌더링 한 뒤 마지막으로 기존 장면과 blur 처리를 한 장면을 합친 뒤 tone mapping을 해주면 된다.

```glsl
#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float exposure;

void main() {
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    hdrColor += bloomColor;                              // additive blending
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure); // tone mapping
    result = pow(result, vec3(1.0 / gamma));             // gamma correction
    FragColor = vec4(result, 1.0);
}
```

### Deferred shading

- 지금까지 한 것처럼 빛이나 물체에 관련된 모든 정보를 쉐이더에게 전달해 준 뒤 한 번에 그리게 하는 방식을 Forward shading이라고 한다. 이 방식의 문제점은 렌더링 해야 할 빛이 여러 개로 늘어날 때 생긴다. 각각의 light마다 fragment shader에서 빛 계산을 해주어야 하기 때문에 연산량이 늘어나 성능이 저하된다. 그리고 화면 밖 오브젝트나 화면에 있지만 가려진 오브젝트에 대한 계산도 하므로 불필요한 계산이 생긴다.
- defer는 미루다 라는 뜻으로 deferred shading은 빛 연산을 뒤로 미룬다는 뜻이다.
- 빛 렌더링에 필요한 오브젝트와 빛, 카메라의 position, normal, albedo(기본 배색), specular 같은 도형이나 텍스쳐 데이터를 먼저 렌더링해 프레임버퍼에 저장해 둔다. 이 프레임버퍼를 G-Buffer라고 한다.
- 그리고 G-Buffer에 저장된 정보를 토대로 나중에 각 픽셀마다 빛 계산을 한다.

#### (1) G-Buffer 만들기

- framebuffer.h 와 framebuffer.cpp 쪽에 코드 수정이 일어났는데, 이는 G-Buffer는 빛 연산을 위한 여러가지 정보들을 여러 개의 컬러 버퍼에 나누어 저장하기 때문에, 프레임버퍼가 여러 개의 컬러 버퍼들을 담을 수 있게 하기 위해 인자로 하나가 아닌 여러 개의 텍스쳐를 받고 glDrawBuffers()를 이용해서 여러 개의 버퍼들을 그려주었다.
- texture.cpp에서는 텍스쳐의 format을 지정해주는 SetTextureFormat()에 변화가 있었다. 컬러 버퍼에 위치를 저장하기 위해서는 큰 범위의 값을 받기 위해 텍스쳐 형식을 GL_RGBA16F 같이 큰 floating point 형태로 지정해 주어야 하는데, 이미지 타입은 GL_RGBA와 같이 기본 형태를 써야 한다. 텍스쳐 형식과 이미지 형식이 달라질 수 있기 때문에 이 함수에 수정을 가한 것이다.
- 새로운 쉐이더인 defer_geo.vert, defer_geo.frag를 추가했다. vertex shader는 lighting.vert와 거의 유사하고, fragment shader는 여러 개의 컬러 버퍼를 반환하기 위해 layout을 이용해서 3개의 컬러 버퍼를 반환할 수 있게 했다. 그리고 각각의 값들은 main() 함수에서 지정해 준다.

```glsl
// G-Buffer를 위한 3개의 output - 3개의 color attachment를 반환
layout (location = 0) out vec4 gPosition; // 위치
layout (location = 1) out vec4 gNormal; // 법선
layout (location = 2) out vec4 gAlbedoSpec; // 기본 배색(RGB)과 반사광(A)

[중략]

void main() {
    // store the fragment position vector in the first gbuffer texture - fragment의 위치를 저장
    gPosition = vec4(position, 1.0);
    // also store the per-fragment normals into the gbuffer - fragment의 법선 벡터 저장
    gNormal = vec4(normalize(normal), 1.0);
    // and the diffuse per-fragment color - 기본 배색을 저장
    gAlbedoSpec.rgb = texture(material.diffuse, texCoord).rgb;
    // store specular intensity in gAlbedoSpec’s alpha component - 반사 정도를 저장
    gAlbedoSpec.a = texture(material.specular, texCoord).r;
}
```

- context.h에는 deferred shading을 위한 변수를 추가하고, context.cpp에서는 쉐이더를 사용하는 부분과 이를 통해 만들어진 G-Buffer를 확인하기 위해 ImGUI 창을 1개 더 만들어 주었다.

- G-Buffer Position
![G-buffer1](/note_image/deferred_shading1.png)
- 위치 값이 저장된 모습으로 0~1 범위만 그려낼 수 있어 음수는 0으로, 1보다 큰 부분은 1로 그려진다.
- 화면에서 오른쪽이 x축 방향, 앞쪽이 z축 방향, 위쪽이 y축 방향이 되고, 텍스쳐 맵에서 각각 빨간색, 파란색, 초록색 값을 결정한다.
- x,y,z좌표가 각각 1 이상인 픽셀은 빨간색, 초록색, 파란색으로 나타나고, 이들이 조합되어서 화면에 마젠타나 시안, 노란색이 나타나는 것이다.

- G-Buffer Normal
![G-buffer2](/note_image/deferred_shading2.png)
- 빨간색 부분은 법선 벡터가 x축 방향에 가까움을 , 초록색 부분은 법선 벡터가 y축 방향에 가까움을, 파란색 부분은 z축 방향에 가까움을 의미한다. (큐브의 면마다 법선 벡터 방향을 다르게 지정했기 때문에 면마다 색이 다르게 나온다.)

- G-Buffer Albedo와 Specular
![G-buffer3](/note_image/deferred_shading3.png)
- Albedo는 텍스쳐에 저장된 픽셀의 색을 그대로 그려낸다. 그런데 상자의 가운데 부분이 검은색인 이유는 상자의 specualr 맵에서 가운데 부분의 반사 정도를 0으로(검게) 지정했기 때문에 상자 가운데 부분은 alpha 값이 0이어서 검게 보이고 나머지 부분은 텍스쳐 맵에서의 원래 색을 들고온다.

#### Deferred Shading 적용 (Lighting Pass)

- 이제 G-Buffer의 데이터를 받아서 빛 계산을 해주는 또다른 쉐이더 프로그램을 만들면 된다.
- defer_light.vert는 이전의 texture.vert와 비슷하다. 텍스쳐에서 픽셀을 받아와 반환한다.
- defer_light.frag는 G-buffer의 데이터를 받기 위해 3개의 sampler2D를 uniform으로 받고, 추가적으로 32개의 빛을 렌더링하기 위해 간단한 형태의 빛의 정보(위치와 색상)를 배열 형태로 받는다.

```glsl
// G-Buffer의 데이터를 텍스쳐 형태로 받는다.
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

// 빛의 위치와 색상 - attenuation(감쇠)이 없는 point light 형태로 구현
struct Light {
    vec3 position;
    vec3 color;
};

// 32개의 빛을 담은 배열
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
```

- 그 다음 main 함수에서 G-Buffer의 데이터를 가져온 뒤 이들을 이용해서 빛 계산을 해 최종적인 픽셀의 색상을 결정한다. 이때 빛 32개를 모두 렌더링 해야 하므로 반복문을 사용해서 빛 계산을 해준다.

```glsl
// retrieve data from G-buffer
vec3 fragPos = texture(gPosition, texCoord).rgb;
vec3 normal = texture(gNormal, texCoord).rgb;
vec3 albedo = texture(gAlbedoSpec, texCoord).rgb;
float specular = texture(gAlbedoSpec, texCoord).a;
// then calculate lighting as usual
// G-Buffer 데이터를 받아 원래 빛을 계산하는 공식을 쓴다.
vec3 lighting = albedo * 0.1; // hard-coded ambient component(주변광)
vec3 viewDir = normalize(viewPos - fragPos);
// 32개의 빛에 대해 빛의 방향을 계산해서 분산광을 계산한다.
for(int i = 0; i < NR_LIGHTS; ++i) {
    // diffuse
    vec3 lightDir = normalize(lights[i].position - fragPos);
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * lights[i].color;
    lighting += diffuse;
}
// 최종 색 반환
fragColor = vec4(lighting, 1.0);
```

- common.h, common.cpp에 임의의 빛을 만들기 위한 간단한 난수 생성 함수를 추가했다.
- 그리고, context.h와 context.cpp에는 lighting pass를 적용하기 위해 새로운 맴버 변수를 추가하고 Init()과 Render()에 코드를 추가했다. G-Buffer에 우리가 그릴 바닥과 큐브에 대한 데이터가 이미 있고, lighting pass 이후에 우리가 그린 코드를 넣으면 렌더링된 32개의 빛이 덮어 쓰여지므로, lighting pass 이후의 모든 렌더링 코드(스카이맵과 큐브 그리는 부분)는 주석 처리했다.
- 실행 결과
![Lighting Pass](/note_image/deferred_shading4.png)
- 화면에서 큐브들의 각 면 색깔이 약간 다른 것을 통해 여러 개의 빛이 렌더링 되었음을 알 수 있다. 그리고 빛이 32개나 있음에도 부드럽게 카메라 이동이 되는 것을 통해 deferred shading의 효과를 볼 수 있다.

#### Forward Rendering과 같이 사용

- Deferred Shading은 많은 빛과 오브젝트가 있어도 이들을 효율적으로 그릴 수 있다는 장점이 있지만, 약점도 존재한다.
- Deferred Shading으로는 blending operation을 할 수 없다. 즉, 반투명한 오브젝트를 그릴 수 없다.
- 모든 픽셀을 하나의 fragment shader로만 그려야 한다. 즉, 오브젝트마다 다른 lighting 기법을 사용하거나 fragment shader를 따로 지정해주는 방식은 쓸 수 없다.
- 그래서 이러한 약점을 보완하기 위해 지금까지 사용한 forward rendering과 같이 사용할 수 있다. 대부분의 Scene은 Deferred shading을 이용한 뒤, 블렌딩이나 UI등을 그리는 데 Forward rendering을 사용한다.
- 둘을 혼용할 때, Deferred shading을 한 부분 바로 뒤에 forward rendering 부분을 넣게 되면 forward rendering 부분이 제대로 그려지지 않게 된다. 왜냐하면, deferred shading을 이용하면 z = 0.5인 평면에 모든 그림을 그리는 형태가 되는데, 이후 그려지는 forward rendering의 물체들이 depth test를 통과하지 못해서 제대로 그려지지 않게 된다. 그래서 forward rendering을 하기 전에 G-Buffer의 depth buffer를 들고올 필요가 있다.

```c++
glBindFramebuffer(GL_READ_FRAMEBUFFER, m_deferGeoFramebuffer->Get());
glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
glBlitFramebuffer(0, 0, m_width, m_height,
    0, 0, m_width, m_height,
    GL_DEPTH_BUFFER_BIT, GL_NEAREST);
glBindFramebuffer(GL_FRAMEBUFFER, 0);
```

- 이 부분 뒤에 32개의 빛의 위치에 작은 큐브를 forward rendering으로 그려서 빛의 위치를 시각화 했다.
![Deferred&Forward](/note_image/deferred_shading5.png)

- (+) 이 예제의 defer_light.frag처럼 빛을 uniform으로 받는 방식은 모든 픽셀에 대해 빛을 계산해야 하므로 빛이 아주 많은 경우에는 비효율적일 수 있다. 이 경우에는 빛의 감쇠를 고려해서 빛이 도달하는 범위를 계산하고 (point light이면 구형, spot light이면 원뿔형) 그 범위 안에 있는 경우에만 빛을 계산하는 방식을 사용할 수도 있다. 더 자세한 내용은 : (<https://learnopengl.com/Advanced-Lighting/Deferred-Shading>)
- (+) 빛이 4개 미만일 경우에는 forward rendering만으로도 충분하고, 그 이상인 경우이거나 Scene에 오브젝트가 많은 경우에는 deferred shading을 이용하는 것이 효율적이다.

### SSAO

- 지금까지 만든 Local illumination model에서는 ambient light(주변광)는 다른 빛이 없어도 기본적으로 있는 빛으로 정의하고 상수값으로 지정했었다.
- 그러나 원래 ambient light란, scene에 포함된 모든 빛들이 물체에 부딫혀 발생된 산란을 시뮬레이션 한 것으로 local illumination model에서는 이를 단순화 했다. 이런 방식은 현실적이지는 않다.
- 그래서 도입된 것이 ambient occlusion(주변광 차폐, AO)으로, 임의의 위치가 차폐된 정도를 계산한 수치이다.(0 ~) 차페된 정도가 높을수록 주변광을 덜 받아 어두울 것이다.
- 이 AO 수치는 각각의 지점에서 적당한 수의 광선을 발사한 뒤 부딫힌 정도를 수집하여 계산한다. 그러나 이 계산은 매우 오래 걸리기 때문에 real-time에서 이 계산을 일일이 하기는 어렵고, normal 맵 처럼 AO 값을 담아놓은 텍스쳐 맵을 사용했었다. (model 파일의 ao.jpg 참고)
- 그러나 2007년 Crytek 사에서 G-buffer를 사용해 AO를 빠르게 근사하는 방법을 발표했는데 이 기법이 SSAO (Screen-Space Ambient Occlusion)이다.
- 아이디어 : 프레임버퍼의 각 픽셀 주위에 여러 개의 샘플들을 만들고, Mesh의 경계를 구해서 이 샘플들 중에서 mesh 안에 있는 것의 비율을 구한다. 그리고 샘플 위치의 depth값이 샘플 위치보다 더 작은 경우 차폐가 발생한 것으로 간주한다.
- 이때 샘플의 수를 잘 조정하는 것이 중요하다. 샘플의 수가 너무 적으면 occlusion이 띠 형태로 나타나는 banding 현상이 나타나고, 너무 많으면 성능 저하가 일어나기 때문이다. 그래서 이 banding 현상을 완화하기 위해 샘플 수집을 위한 커널을 회전시킨 결과를 blur 처리해서 품질을 높일 수가 있다.
- 커널의 형태는 normal 방향을 기준으로 회전된 반구 형태를 사용한다. 완전 구형의 커널을 사용하게 되면 평평한 면에서 절반의 차폐가 일어나기 때문이다.
![SSAO1](/note_image/SSAO1.jpg)

#### SSAO를 이용한 AO map 그리기

- SSAO를 하기 위해서는 G-Buffer, Sample kernel(반구 형태 안에서의 기준 위치), random rotation vector가 필요하다.
- G-Buffer의 경우 앞에서 만든 것을 사용하면 된다.
- random rotation vector는 난수 생성을 통해 16개의 난수 벡터를 만들고, 이것으로 이루어진 4x4 크기의 텍스쳐 맵을 만든다.

```c++
// random rotation vector 생성
std::vector<glm::vec3> ssaoNoise;
ssaoNoise.resize(16);
for (size_t i = 0; i < ssaoNoise.size(); i++) {
    // randomly selected tangent direction
    // x, y만 난수인 이유는 z축에 가까운 normal vector에 대한 임의의 tangent vector를 만들어 내기 위해서이다.
    glm::vec3 sample(RandomRange(-1.0f, 1.0f), RandomRange(-1.0f, 1.0f), 0.0f);
    ssaoNoise[i] = sample;
}

// 4x4 크기의 텍스쳐 맵에 저장
m_ssaoNoiseTexture = Texture::Create(4, 4, GL_RGB16F, GL_FLOAT);
m_ssaoNoiseTexture->Bind();
m_ssaoNoiseTexture->SetFilter(GL_NEAREST, GL_NEAREST);
m_ssaoNoiseTexture->SetWrap(GL_REPEAT, GL_REPEAT);
// 이미 바인딩된 텍스쳐의 데이터를 바꾸기 위해 glTexSubImage2D를 사용한다.
glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 4, 4,
    GL_RGB, GL_FLOAT, ssaoNoise.data());
```

- Sample kernel은 난수 함수를 이용해서 z축을 normal vector로 하는 지름이 1인 반구 내에 임의의 지점을 골라서 저장한다. 이후 원점에 점들이 가까울 수록 차폐 연산의 결과가 더 좋기 때문에 2차 함수 형태의 scale 값을 구해서 위치 벡터에 곱해준다.

```c++
// SSAO를 위한 커널 샘플의 위치 저장
m_ssaoSamples.resize(64);
for (size_t i = 0; i < m_ssaoSamples.size(); i++) {
    // uniformly randomized point in unit hemisphere
    glm::vec3 sample(
        RandomRange(-1.0f, 1.0f),
        RandomRange(-1.0f, 1.0f),
        RandomRange(0.0f, 1.0f));
    sample = glm::normalize(sample) * RandomRange();

    // scale for slightly shift to center
    // 더 좋은 차폐 연산을 위해서 모든 점들이 중심에 조금 더 가깝게 배치되도록 위치를 조절
    float t = (float)i / (float)m_ssaoSamples.size();
    float t2 = t * t;
    float scale = (1.0f - t2) * 0.1f + t2 * 1.0f;

    m_ssaoSamples[i] = sample * scale;
}
```

- 이제 쉐이더를 살펴보자. vertex shader는 지난 번에 만든 defer_light.vert와 동일하다.
- fragment shader에서는 입력으로는 vertex shader에서 텍스쳐 좌표, G-Buffer에서 position, normal을 받는다. texNoise는 랜덤 회전 벡터로 만든 4x4 텍스쳐를 의미한다. 그리고 좌표 계산을 위한 view, projection matrix, 샘플을 저장하는 반구의 반지름, 랜덤 샘플들을 저장하는 배열을 받는다.

```glsl
// AO값만 출력하기 위해 out 변수가 float이다.
out float fragColor;

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform mat4 view;
uniform mat4 projection; // Screen-space 계산을 위해 필요

uniform vec2 noiseScale;
uniform float radius; // 샘플들을 저장하는 반구의 반지름

// random samples
const int KERNEL_SIZE = 64;
const float BIAS = 0.025; // acne 방지를 위한 편향값
uniform vec3 samples[KERNEL_SIZE]; // 반구 내 임의의 위치를 담은 배열
```

- main 함수에서는 아래와 같은 과정을 거친다. (코드는 ssao.frag를 참고)

> 1. 월드 좌표에 view matrix를 곱해 카메라 시선을 기준으로 한 픽셀의 좌표와 법선 벡터를 가져온다.
> 2. 랜덤 회전 벡터 값을 texNoise에서 가져온다.
> 3. Gram-Schmidt 과정을 이용해 randomVec를 normal vector에 수직한 평면에 내린 정사영인 tangent 벡터를 구하고, 이를 이용해 TBN 행렬을 계산한다.
> 4. 픽셀 위치를 중심으로 하고 반지름이 radius인 반구 내 임의의 위치(샘플)를 가져온 뒤 그것의 screen-space상 좌표를 구한다.
> 5. 화면 상 좌표에서 그려진 픽셀의 z값과 샘플의 z값을 비교해서 차폐의 여부를 검증한다. 그려진 픽셀의 z값이 샘플의 z값보다 큰 경우, 샘플이 매쉬 안쪽에 있다는 뜻이므로 차폐 값에 1 / (총 샘플의 수)를 누적하고, 아닌 경우에는 0을 누적한다.
> 6. 추가적으로 샘플과 중심 사이의 거리를 계산해서 샘플이 중심으로 부터 너무 멀리 떨어진 경우 이 샘플이 주는 영향을 줄이기 위해서 rangeCheck을 구해 차폐 값에 곱해준다.
> 7. 1 - (구한 차폐값)이 최종 AO 값이 된다.

- SSAO 프로그램을 사용하는 부분은 Deferred shading 에서 geometry pass를 통해 G-Buffer를 생성한 부분 바로 뒤에 붙이면 된다.

```c++
m_ssaoFramebuffer->Bind();
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glViewport(0, 0, m_width, m_height);

m_ssaoProgram->Use();
glActiveTexture(GL_TEXTURE0);
m_deferGeoFramebuffer->GetColorAttachment(0)->Bind();
glActiveTexture(GL_TEXTURE1);
m_deferGeoFramebuffer->GetColorAttachment(1)->Bind();
glActiveTexture(GL_TEXTURE2);
m_ssaoNoiseTexture->Bind();
glActiveTexture(GL_TEXTURE0);

m_ssaoProgram->SetUniform("gPosition", 0);
m_ssaoProgram->SetUniform("gNormal", 1);
m_ssaoProgram->SetUniform("texNoise", 2);
m_ssaoProgram->SetUniform("noiseScale", glm::vec2(
    (float)m_width / (float)m_ssaoNoiseTexture->GetWidth(),
    (float)m_height / (float)m_ssaoNoiseTexture->GetHeight()));
m_ssaoProgram->SetUniform("radius", m_ssaoRadius);
for (size_t i = 0; i < m_ssaoSamples.size(); i++) {
    auto sampleName = fmt::format("samples[{}]", i);
    m_ssaoProgram->SetUniform(sampleName, m_ssaoSamples[i]);
}
m_ssaoProgram->SetUniform("transform", glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));
m_ssaoProgram->SetUniform("view", view);
m_ssaoProgram->SetUniform("projection", projection);
m_plane->Draw(m_ssaoProgram.get());
```

- 그려진 SSAO의 모습
![SSAO2](/note_image/SSAO2.png)
- 물체에 가까이 다가가보면 작은 패턴이 생김을 확인할 수 있는데 이는 우리가 4x4 크기의 랜덤 회전 벡터 텍스쳐맵을 이어 붙이는 형태로 사용했기 때문이다.
![SSAO3](/note_image/SSAO3.png)

#### SSAO에 blur 효과 주기

- 이제 만들어진 AO map에다 blur 효과를 적용해 볼 것이다.
- 새로운 쉐이더 blur_5x5.vert와 .frag를 만든다. vertex shader는 ssao.vert와 같다.
- fragment shader에서는 주변 5x5 영역의 이웃 텍스쳐들의 픽셀 색상을 모두 구해 그것의 평균을 적용해서 blur 효과를 낸다.

```glsl
void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(tex, 0));
    vec4 result = vec4(0.0);
    // 주변 5x5의 픽셀 색깔의 평균을 적용한다.
    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(tex, texCoord + offset);
        }
    }
    fragColor = result / 25.0;
}
```

- context.h에 blur 효과를 적용할 쉐이더 프로그램과 이를 저장할 프레임버퍼 변수를 선언하고 context.cpp에서 초기화 해준 뒤, SSAO 맵을 그리는 코드 다음에 blur 효과를 적용해준다.

```c++
m_ssaoBlurFramebuffer->Bind();
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glViewport(0, 0, m_width, m_height);
m_blurProgram->Use();
m_ssaoFramebuffer->GetColorAttachment(0)->Bind();
m_blurProgram->SetUniform("tex", 0);
m_blurProgram->SetUniform("transform",
    glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));
m_plane->Draw(m_blurProgram.get());
```

- blur 처리된 SSAO 맵의 모습
![SSAO4](/note_image/SSAO4.png)
- 확대해서 보면 blur 처리로 패턴이 흐려진 것을 확인할 수 있다. (왼쪽이 blur 적용 전, 오른쪽이 적용 후)
![SSAO5](/note_image/SSAO5.png)

#### 실제 빛 렌더링에 적용

- 만들어진 SSAO 맵을 이제 deferred shading을 이용한 빛 렌더링에 적용해 볼 것이다.
- defer_light.frag 파일을 수정해서 SSAO 맵을 직접 받을 수 있도록 한다. main 함수에서는 주변광을 하드코드하는 부분을 useSsao가 1이면 SSAO 맵의 텍스쳐에서 가져온 색상에 0.4를 곱한 것을 주변광으로 정하도록 한다.

```glsl
// SSAO 맵을 받는다.
uniform sampler2D ssao;
uniform int useSsao; // SSAO 맵 적용 여부

// main 함수
// 주변광 계산 (useSsao가 1이면 SSAO 맵을 사용, 아니면 상수 주변광 사용)
vec3 ambient = useSsao == 1 ?
    texture(ssao, texCoord).r * 0.4 * albedo :
    albedo * 0.4; // hard-coded ambient component 
vec3 lighting = ambient; 
```

- context.h에는 SSAO 맵을 사용할 지 여부를 저장하는 bool 맴버를 추가하고, context.cpp에서 Init() 함수의 빛을 만드는 부분에서 SSAO 맵의 효과를 보기 위해 3개의 빛에만 색을 부여하는 코드를 넣는다.
- lighting pass 부분에 새로운 uniform 변수를 받기 위해 SSAO 맵을 3번 텍스쳐에 바인딩하고, useSsao를 전달해주는 과정을 추가하면 된다.

```c++
glActiveTexture(GL_TEXTURE3);
m_ssaoBlurFramebuffer->GetColorAttachment()->Bind(); // SSAO map
// [...]
m_deferLightProgram->SetUniform("ssao", 3);
m_deferLightProgram->SetUniform("useSsao", m_useSsao ? 1 : 0);
```

- 실행한 뒤 SSAO 맵을 끈 것과 켠 것을 비교하면 SSAO 맵을 켠 쪽이 가장자리에 약간의 음영이 있어서 현실적인 느낌을 준다. (왼쪽이 SSAO를 켠 쪽, 오른쪽이 끈 쪽이다.)
![SSAO6](/note_image/SSAO6.png)
- (+) 렌더링 하려는 장면마다 sample의 개수, radius, bias를 잘 조절해 주어야 좋은 효과를 낼 수 있다.
- (+) 마지막 과정에서 fragColor를 결정할 때 지수 함수를 사용할 수도 있다.

```glsl
occlusion = 1.0 - (occlusion / KERNEL_SIZE);
fragColor = pow(occlusion, power);
```

### PBR (physical based rendering)

- 물리적인 방식을 근사하여 빛을 모사하는 방식으로 Phong shading보다 더 사실적인 방식이다.
- Microfacet model, 에너지 보존 법칙, BRDF를 구현해야 PBR이라고 할 수 있다.
- 물리적인 파라미터로 물체의 재질을 디자인할 수 있다.

#### Microfacet model

- 현실에서 모든 표면을 확대해보면 매우 작은 완전 반사 거울면으로 이루어져 있는데 이 면들을 미세면(microfacet)라고 한다.
- 거친면에서는 반사광이 분산되어 빛에 의한 하이라이트가 넓어지고, 부드러운 면에서는 반사광이 균일해서 빛에 의한 하이라이트가 좁다.
- 미세면은 너무 작기 때문에 컴퓨터 픽셀 레벨에서 구분하는 것은 불가능하다. 그래서 그래픽스에서는 roughness라는 표면의 거친 정도를 나타내는 파라미터를 이용해서 면의 거친 정도를 통계적으로 근사해서 표현한다. roughness가 클 수록 거친 면인 것이다. (roughness의 정확한 정의는 halfway 벡터에 정렬된 미세면의 비율을 뜻한다.)

#### 에너지 보존 법칙(Energy conservation)

- PBR에서의 에너지 보존 법칙은 출력되는 빛의 에너지는 입력되는 빛의 에너지를 초과할 수 없다는 것이다.
- 빛의 하이라이트 면적이 클 수록 밝기가 줄어드는 이유가 이것 때문이다.
- 입사광이 물체에 닿으면 굴절광과 반사광으로 나뉘게 된다. 이때 이들의 합산은 입사한 빛의 에너지를 초과할 수 없다.
- 굴절된 빛은 표면의 입자와 충돌한 뒤 흡수되어 열 에너지로 전환되거나 임의의 방향으로 반사될 수도 있다.
- 이때 빛이 반사되거나 굴절되는 정도는 물체의 재질마다 다를 것이다. 유전체(dielectric, 부도체라고도 함)에서는 입사한 빛 중 일부가 재질 표면 입자에 부딪히다 출력되고, 금속 재질의 물체에서는 굴절광이 모두 흡수되어 분산광 색이 없어진다.
- 여기서 고려할 수 있는 또 다른 파라미터가 바로 metallic(금속성)으로, 물체가 얼마나 금속에 가까운 지를 나타내는 수치이다. 0이면 완전 비금속, 1이면 완전 금속성을 띄는 물체로 base color의 영향을 거의 받지 않고 반사되는 빛의 색을 띄게 된다.
- 에너지 보존 법칙을 고려하여 반사광과 분산광을 계산할 때는 후술할 BRDF를 이용해 specular의 비율을 구한뒤 그 비율을 1에서 빼는 방식으로 diffuse의 비율을 구한다.

#### Reflectance equation

- 특정 위치 p에서 출사 방향 벡터(빛이 반사되어 나오는 방향) $\omega_o$ 로 나오는 빛의 에너지 크기를 나타내는 방정식이다. - p를 중심으로 하는 반구 내에 들어오는 모든 빛의 방향을 고려하므로 입사각에 대한 적분 형태로 나타낸다.
![PBR1](/note_image/PBR1.jpg)
- Radiance는 광원으로부터 $\Phi$만큼의 에너지가 $\theta$의 각도로 입사했을 때 단위 면적당 방출된 전자기파의 양으로 아래의 식으로 나타낸다. $\omega$ 는 단위 구에 투영한 면적과 각도를 가진 도형이고, I는 단위 구체 상의 면적당 광원의 세기를 뜻한다.
![PBR2](/note_image/PBR2.jpg)
- 이 공식을 쉐이더에서 구현할 때에는 리만 합의 형태로 for 문을 사용해서 계산한다.

```glsl
int steps = 100;
float sum = 0.0f;
vec3 P = ...;
vec3 Wo = ...;
vec3 N = ...;
float dW = 1.0f / steps;
for(int i = 0; i < steps; ++i) {
    vec3 Wi = getNextIncomingLightDir(i);
    sum += Fr(P, Wi, Wo) * L(P, Wi) * dot(N, Wi) * dW;
}
```

#### BRDF (Bidirectional Reflective Distribution Function)

- BRDF (양방향 반사도 분포 함수)는 특정 입사각으로 들어온 광선이 반사광에 얼마나 기여하는 지에 대한 분포 함수이다. 입사각, 출사각, normal vector, roughness를 입력으로 받는다. (물체의 재질마다 다르다.)
![PBR3](/note_image/PBR3.jpg)
- $k_{d}$ 는 굴절되어 들어오는 빛의 비율, $k_{s}$ 는 반사되어 나가는 빛의 비율이다. $f_{lambert}$ 는 램버트 반사율로 아래와 같이 정의된다.
![PBR4](/note_image/PBR4.jpg)
- 여기서 c는 물체의 albedo - 기본색(RGBA)를 나타낸다.
- $f_{cook-torrance}$ 는 3가지 함수의 곱을 정규화한 형태로 아래와 같이 구성되어 있다.
![PBR5](/note_image/PBR5.jpg)
- Normal Distribution Function는 표면의 거칠기에 따라 미세면들이 얼마나 halfway vector와 정렬되어 있는지를 계산하는 함수이다. halfway vector와 미세면의 법선 벡터가 일치하면 거울면 반사가 일어나므로 Roughness 수치가 작을수록 정반사가 커져 주변 환경이 잘 비추어 보일 것이다. 통계적 근사 함수인 Trowbridge-Reitz GGX를 사용해서 표현한다.
![PBR6](/note_image/PBR6.jpg)

```glsl
float DistributionGGX(vec3 N, vec3 H, float a) {
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}
```

- Geometry Function는 미세면으로 인해 shadowing과 masking이 발생하는 정도를 계산해주는 함수이다. 미세면이 거칠수록 빛이 미세면 안에서 그림자가 지거나 반사된 빛이 다른 미세면에 가려서 카메라 시선에서는 보이지 않게 될 가능성이 있다. 이 함수가 0이면, 미세면에 의해 반사된 빛이 막힌다는 뜻이고, 1이면 미세면에 의해 막히는 빛이 없다는 뜻이다. 근사 함수인 SchlickGGX를 사용하는데, 관찰자가 보는 방향과, 빛의 방향 모두를 고려하는 것이 좋으므로 이 근사 함수를 각각 v와 l에 대해 적용한 것을 곱해서 사용한다.
![PBR7](/note_image/PBR7.jpg)

```glsl
float GeometrySchlickGGX(float NdotV, float k) {
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
    return ggx1 * ggx2;
}
```

- Fresnel Equation는 서로 다른 각도의 표면에서 반사되는 빛의 비율을 계산하는 함수이다. Fresnel 현상이란, 물체를 거의 직각 방향으로 바라보았을 때 specualr light가 보다 밝게 보이는 현상을 말한다. $F_{0}$는 표면의 기본 반사율로 재질마다 다르다. 근사 함수인 Schlick Approximation으로 나타낸다.
![PBR8](/note_image/PBR8.jpg)

- 재질에 따른 기본 반사율 (<https://learnopengl.com/PBR/Theory>의 표에서 발췌했다.)

| 재질 | $F_{0}$ (Linear) | $F_{0}$ (sRGB) |
|---|---|---|
| 물 | (0.02, 0.02, 0.02) | (0.15, 0.15, 0.15)|
| 플라스틱/유리 | (0.03, 0.03, 0.03) | (0.21, 0.21, 0.21) |
| 다이아몬드 | (0.17, 0.17, 0.17) | (0.45, 0.45, 0.45) |
| 철 | (0.56, 0.57, 0.58) | (0.77, 0.78, 0.78) |
| 구리 | (0.95, 0.64, 0.54) | (0.98, 0.82, 0.76) |
| 금 | (1.00, 0.71, 0.29) | (1.00, 0.86, 0.57) |
| 알루미늄 | (0.91, 0.92, 0.92) | (0.96, 0.96, 0.97) |
| 은 | (0.95, 0.93, 0.88) | (0.98, 0.97, 0.95) |

- 코드에서는 metallic 수치를 사용하는데, 비금속의 평균 $F_{0}$ 값인 0.04와 선형 보간한 값을 적용한다.

```glsl
vec3 fresnelSchlick(float cosTheta, vec3 surfaceColor, float metallic) {
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, surfaceColor.rgb, metallic);
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
```

- 이를 모두 적용하면 Reflectance equation을 아래와 같이 다시 쓸 수 있다.
![PBR9](/note_image/PBR9.jpg)
- 위의 수식에서 $k_{s}$가 빠져 있는 이유는 $F$에 $k_{s}$가 내장되어 있기 때문이다.

> Source<br>
> <https://learnopengl.com/PBR/Theory><br>
> <https://bbtarzan12.github.io/PBR/>

#### PBR 재질의 파라미터

- Albedo(물체 기본 색상), Normal(법선), Metallic(금속성), Roughness(거칠기), Ambient Occlusion(주변광 차페)을 파라미터로 갖는다.

#### PBR 구현 (<https://www.youtube.com/watch?v=HuGEcaMpqEQ&list=PLvNHCGtd4kh_cYLKMP_E-jwF3YKpDP4hf&index=42>)

- 코드 간소화를 위해 context.h와 context.cpp에서 핵심적인 내용을 제외한 거의 모든 렌더링 코드와 맴버를 지웠다.
- PBR 재질의 파라미터 값의 변화를 잘 확인하기 위해 구 형태의 매시를 그리기 위해 mesh.h, mesh.cpp에 구를 그리는 코드를 추가했다. context.cpp의 DrawScene에서는 구를 7x7의 격자 형태로 배치했다. 그리고 각각의 구마다 roughness와 metallic 수치를 다르게 했다.

```c++
program->Use();
 
const int sphereCount = 7;
const float offset = 1.2f;
// 7x7 배치로 49개의 구를 그린다.
for (int j = 0; j < sphereCount; j++) {
    float y = ((float)j - (float)(sphereCount - 1) * 0.5f) * offset;
    for (int i = 0; i < sphereCount; i++) {
        float x = ((float)i - (float)(sphereCount - 1) * 0.5f) * offset;
        auto modelTransform =
            glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
        auto transform = projection * view * modelTransform;
        program->SetUniform("transform", transform);
        program->SetUniform("modelTransform", modelTransform);
        program->SetUniform("material.roughness",
            (float)(i + 1) / (float)sphereCount);
        program->SetUniform("material.metallic",
            (float)(j + 1) / (float)sphereCount);
        m_sphere->Draw(program);
    }
}
```

- PBR 구현을 위한 쉐이더를 추가했다. vertex shader는 lighting.vert와 거의 동일하고, fragment shader에서는 위에서 설명한 Reflectance equation과 BRDF를 코드로 구현했다.
- context.cpp의 Render 함수에서 새로운 프로그램을 사용하는 부분을 추가한다.

```c++
m_pbrProgram->Use();
m_pbrProgram->SetUniform("viewPos", m_cameraPos);
m_pbrProgram->SetUniform("material.albedo", m_material.albedo);
m_pbrProgram->SetUniform("material.ao", m_material.ao);
for (size_t i = 0; i < m_lights.size(); i++) {
    auto posName = fmt::format("lights[{}].position", i);
    auto colorName = fmt::format("lights[{}].color", i);
    m_pbrProgram->SetUniform(posName, m_lights[i].position);
    m_pbrProgram->SetUniform(colorName, m_lights[i].color);
}
DrawScene(view, projection, m_pbrProgram.get());
```

- 실행한 모습 (설명을 위해 그림 편집)
![PBR10](/note_image/PBR10.png)
- 왼쪽에서 오른쪽으로 갈 수록 roughness가 증가하고, 아래에서 위로 갈수록 metallic이 증가한다. roughness가 증가할 수록 빛에 의한 하이라이트 면적이 넓어짐을 확인할 수 있고, metallic이 증가할 수록 물체의 고유 albedo 보다는 주변 환경에 의한 모습이 물체에 비춰지는 것을 확인할 수 있다.(현재는 주변 환경이 검은 배경이기 때문에 metallic이 높은 물체일 수록 검게 보인다.)
- 물체들을 뒤에서 보면 Fresnel 효과에 의해 가장자리에 하이라이팅이 됨을 확인할 수 있다.(albedo를 빨강색으로 함)
![PBR11](/note_image/PBR11.png)

#### PBR 텍스쳐 맵 이용하기

- 이제 PBR 텍스쳐를 가져와서 그려보는 것을 할 것이다.
- pbr_texture 쉐이더를 만든다. vertex shader에서는 normal map을 추가로 받고 쉐이더 안에서 TBN 행렬을 계산해서 반환한다.

```glsl
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

[...]

mat4 invTransModelTransform = transpose(inverse(modelTransform));
vec3 normal = normalize((invTransModelTransform * vec4(aNormal, 0.0)).xyz);
vec3 tangent = normalize((invTransModelTransform * vec4(aTangent, 0.0)).xyz);
vec3 binormal = cross(normal, tangent);
TBN = mat3(tangent, binormal, normal);
```

- fragment shader에서는 텍스쳐에서 재질의 정보를 받기 위해서 Material struct를 수정하고 main 함수의 초반부분을 수정했다.

```glsl
// PBR 머터리얼 파라미터
struct Material {
    sampler2D albedo;
    sampler2D metallic;
    sampler2D roughness;
    sampler2D normal;
    float ao;
};

[...]

// 텍스쳐 맵에서 albedo, metallic, roughness, normal을 가져온다.
vec3 albedo = pow(texture(material.albedo, texCoord).rgb, vec3(2.2)); // sRGB를 linear로 변환
float metallic = texture(material.metallic, texCoord).r;
float roughness = texture(material.roughness, texCoord).r;
float ao = material.ao;
vec3 fragNormal = texture(material.normal, texCoord).rgb * 2.0 - 1.0;
fragNormal = TBN * fragNormal;
```

- context 클래스에서는 텍스쳐맵을 받기 위한 맴버를 추가하고, 텍스쳐 이미지를 불러오며 불러온 이미지를 바인딩해서 쉐이더에 전달해주는 코드가 추가되었다.

- 실행한 모습
![PBR12](/note_image/PBR12.png)

### IBL (Image Based Lighting)

- Enviornment map을 하나의 큰 광원으로 사용하는 기술이다.(주변 배경으로부터의 빛을 적용하는 기술)
- cubemap을 이루는 각 픽셀이 광원이 된다.
- IBL을 사용하면 물체가 주변 배경 안에 있는 느낌을 살려낼 수 있다.
- IBL에서는 cubemap의 각 픽셀이 광원이 되기 때문에 모든 방향에서 오는 빛을 고려해야 한다.

#### IBL - Diffuse irradiance

- 우리가 앞에서 보았던 reflectance equation에서 적분 식을 diffuse term과 specular term으로 분해했을 때 diffuse term을 diffuse irradiance라고 한다.

![PBR16](/note_image/PBR16.jpg)

- p를 enviornment map의 한가운데 지점이라고 가정하면 적분식은 $\omega_{i}$ 에 대한 식으로 볼 수 있다.
- 출력 방향 $\omega_{o}$ 에 대해서 반구 내 모든 입력 방향 $\omega_{i}$ 에 대한 빛의 가중치(코사인 값)를 합산함으로써 Diffuse irradiance를 계산할 수 있다.

##### HDR 맵

- 일반적인 enviornment map으로는 제대로 된 irradiance map을 만들어 낼 수 없는데, 일반적인 텍스쳐는 색상 값이 0~1사이로 제한되기 때문이다. 그래서 1 이상의 값을 저장할 수 있는 hdr 포맷의 enviornment map을 사용한다.
- hdr 포맷의 이미지를 가져오기 위해서는 채널의 크기가 float (4바이트)인 이미지를 읽을 수 있도록 지금까지 만든 코드에 수정이 필요하다. image.h와 image.cpp에서 채널 당 바이트 수를 저장하는 새로운 맴버를 만들고, 파일형식이 .hdr이면 채널 당 바이트 수를 4로 지정하는 부분을 새로 추가했다.

```c++
auto ext = filepath.substr(filepath.find_last_of('.'));
// hdr 형식의 이미지는 채널이 float이다.
if (ext == ".hdr" || ext == ".HDR") {
    m_data = (uint8_t*)stbi_loadf(filepath.c_str(),
    &m_width, &m_height, &m_channelCount, 0);
    m_bytePerChannel = 4;
}
else {
    m_data = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channelCount, 0);
    m_bytePerChannel = 1;
}
```

- texture 클래스에서도 image로 부터 텍스쳐를 만들어내는 SetTextureFromImage() 함수에서 이미지의 채널 당 바이트 수가 4인 경우 16F 형식으로 지정하도록 수정했다.

```c++
// .hdr과 같이 채널 하나가 float인 이미지 형식의 경우에는 16F 형식으로 지정한다.
if (image->GetBytePerChannel() == 4) {
    m_type = GL_FLOAT;
    switch (image->GetChannelCount()) {
        default: break;
        case 1: m_format = GL_R16F; break;
        case 2: m_format = GL_RG16F; break;
        case 3: m_format = GL_RGB16F; break;
        case 4: m_format = GL_RGBA16F; break;
    }
}
```

- HDR 맵은 구의 표면을 하나의 평면에 펼쳐놓은 형태인 equirectangular로 이 자체로 enviornment map으로 바꿀 수는 없다. 그래서 구면 좌표계의 한 점을 직교 좌표계의 점으로 바꿔서 cubemap 형태로 변환하는 spherical_map 쉐이더를 만든다.
- vertex shader

```glsl
#version 330 core
// skybox.vert와 거의 비슷함
layout (location = 0) in vec3 aPos;
out vec3 localPos;
uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    localPos = aPos;
}
```

- fragment shader

```glsl
#version 330 core
out vec4 fragColor;
in vec3 localPos;
uniform sampler2D tex;

const vec2 invPi = vec2(0.1591549, 0.3183098862);
// 텍스쳐 좌표로 변환
vec2 SampleSphericalMap(vec3 v) {
    return vec2(atan(v.z, v.x), asin(v.y)) * invPi + 0.5;
}

void main() {
    vec2 uv = SampleSphericalMap(normalize(localPos)); // normalize
    vec3 color = texture(tex, uv).rgb;
    fragColor = vec4(color, 1.0);
}
```

- context.h에 m_hdrMap, m_sphericalMapProgram맴버를 추가하고, context.cpp에서 두 맴버를 초기화한 뒤 hdr 맵을 큐브맵 텍스쳐 형태로 변환한 것을 큐브에 그리도록 한다. (이것은 중간 과정을 위한 코드로 Render() 함수에서 사용되었다. 완성된 코드에는 빠져있다.)

```c++
m_sphericalMapProgram->Use();
m_sphericalMapProgram->SetUniform("transform",
projection * view *
glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.0f)));
m_sphericalMapProgram->SetUniform("tex", 0);
m_hdrMap->Bind();
m_box->Draw(m_sphericalMapProgram.get());
```

- 기존의 큐브맵은 6장의 이미지로부터 만들어내지만, hdr 맵은 1장의 사진에서 cubemap을 가져오는 것이므로, 다른 방식으로 큐브맵을 불러올 필요가 있다. 그래서 타입만 지정된 빈 텍스쳐를 만든 뒤, 그 텍스쳐에 이미지 데이터를 붙이는 방식을 사용해서 변환된 hdr 맵을 cube map으로 바꾸어준다. (texture 파일에서 수정) 그리고 hdr은 채널이 4바이트 이므로, 이미지 형식에 따라 채널 당 바이트 맴버를 수정할 수 있도록 한다.

```c++
CubeTextureUPtr CubeTexture::Create(int width, int height,
    uint32_t format, uint32_t type) {
    auto texture = CubeTextureUPtr(new CubeTexture());
    texture->Init(width, height, format, type);
    return std::move(texture);
}
```

- 그리고 큐브 맵을 color attachment로 사용하는 또 다른 프레임버퍼도 필요하다. 그래서 framebuffer 파일에 새로운 클래스인 CubeFramebuffer를 추가했다.
- 이후 enviornment map을 구현하기 위한 새로운 skybox_hdr 쉐이더를 만들어 준다.
- vertex shader에서는 view matrix를 mat3 -> 다시 mat4로 변환하는 과정을 통해 rotation에 대한 정보만 남긴 뒤 그것을 projection matrix과 곱해준다. 그리고 gl_Position에는 xyww를 넣는데, 이렇게 되면 모든 z값에는 1이 들어가게 되고, depth test에 의해 enviornment map의 모든 픽셀은 항상 뒤에 그려지게 된다. 단, z = 1이므로 depth test의 규칙은 GL_LEQUAL로 지정해주어야 제대로 그려진다.

```glsl
#version 330 core

layout (location = 0) in vec3 aPos;
out vec3 localPos;

uniform mat4 projection;
uniform mat4 view;

void main() {
    localPos = aPos;
    mat4 rotView = mat4(mat3(view)); // remove translation - rotation만 남게 된다.
    vec4 clipPos = projection * rotView * vec4(localPos, 1.0);
    gl_Position = clipPos.xyww; // skybox가 항상 뒤에 그려지게 된다.
}
```

- fragment shader에서는 텍스쳐의 픽셀을 그려주는 역할을 하는데, hdr 맵이라서 범위가 1보다 클 수 있기 때문에 이를 0~1 범위로 다시 좁혀주어야 한다. 그래서 reinhard tone mapping을 사용해서 0 ~ 1의 범위로 지정해준뒤 1.0 / 2.2 승을 해주어 linear를 sRGB로 변환해준다.

```glsl
#version 330 core

out vec4 fragColor;
in vec3 localPos;

uniform samplerCube cubeMap;

void main() {
    vec3 envColor = texture(cubeMap, localPos).rgb;
    envColor = envColor / (envColor + vec3(1.0));   // reinhard
    envColor = pow(envColor, vec3(1.0/2.2));    // to sRGB
    fragColor = vec4(envColor, 1.0);
}
```

- context 파일에 필요한 맴버를 추가하고 아래처럼 렌더링 코드를 넣어주면 환경 맵이 잘 나타나는 것을 확인할 수 있다.

```c++
glDepthFunc(GL_LEQUAL);
m_skyboxProgram->Use();
m_skyboxProgram->SetUniform("projection", projection);
m_skyboxProgram->SetUniform("view", view);
m_skyboxProgram->SetUniform("cubeMap", 0);
m_hdrCubeMap->Bind();
m_box->Draw(m_skyboxProgram.get());
glDepthFunc(GL_LESS);
```

![PBR13](/note_image/PBR13.png)

##### Diffuse irradiance 구현

- enviornment map을 그렸으므로 이제 Diffuse irradiance를 구현해서 주변 환경의 빛이 물체에 은은하게 비춰지도록 할 것이다.
- vertex shader는 skybox_hdr.vert를 그대로 사용하고, diffuse만을 위한 fragment shader를 만들어준다. 이 쉐이더에서는 텍스쳐에서 일정 간격으로 픽셀을 가져와 흐린 버전의 irradiance map을 만들어내는 역할을 한다.
- 그리고 context.cpp의 Init() 함수에서 이것을 사용해서 irradiance map을 만든다.

```c++
m_diffuseIrradianceProgram = Program::Create(
    "./shader/skybox_hdr.vert", "./shader/diffuse_irradiance.frag");
m_diffuseIrradianceMap = CubeTexture::Create(64, 64, GL_RGB16F, GL_FLOAT);
cubeFramebuffer = CubeFramebuffer::Create(m_diffuseIrradianceMap);
glDepthFunc(GL_LEQUAL);
m_diffuseIrradianceProgram->Use();
m_diffuseIrradianceProgram->SetUniform("projection", projection);
m_diffuseIrradianceProgram->SetUniform("cubeMap", 0);
m_hdrCubeMap->Bind();
glViewport(0, 0, 64, 64);
for (int i = 0; i < (int)views.size(); i++) {
    cubeFramebuffer->Bind(i);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_diffuseIrradianceProgram->SetUniform("view", views[i]);
    m_box->Draw(m_diffuseIrradianceProgram.get());
}
glDepthFunc(GL_LESS);
```

- 테스트 목적으로 irradiance map을 그려보면 흐린 형태의 배경이 나온다.
![PBR14](/note_image/PBR14.png)

- 마지막으로 pbr.frag를 수정해서 이 irradiance map을 물체에 적용하는 일만 남았다. irradiance map을 받을 수 있도록 새로운 uniform 변수를 추가하고, irradiance map 모드일 때 사용할 다른 fresnel 함수를 정의한 후 useIrradiance가 1일 때 사용하도록 코드를 추가한다.

```glsl
uniform samplerCube irradianceMap;
uniform int useIrradiance;

// irradiance에서 사용하는 fresnel 함수
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

[...]

if (useIrradiance == 1) {
    vec3 kS = FresnelSchlickRoughness(dotNV, F0, roughness);
    vec3 kD = 1.0 - kS;
    vec3 irradiance = texture(irradianceMap, fragNormal).rgb;
    vec3 diffuse = irradiance * albedo;
    ambient = (kD * diffuse) * ao;
}
```

- 이제 코드를 실행하면 아래처럼 된다. 위의 사진이 diffuse irradiance를 켠 모습, 아래 사진이 끈 모습이다. 위의 사진에서 물체에 배경의 빛이 비추어서 약간 더 밝은 것을 확인할 수 있다.
![PBR15-1](/note_image/PBR15-1.png)
![PBR15-2](/note_image/PBR15-2.png)

#### IBL - Specular

- Specular term의 경우 diffuse term과 다르게 미리 계산해 줄 수 없다. 그래서 정확한 계산 대신 이를 Light part와 BRDF part로 나누어 계산한 뒤 이를 곱해서 적용하는 방식으로 미리 계산할 수 있는데 이를 Epic Games' split sum approximation라고 한다.

![PBR17](/note_image/PBR17.jpg)

##### Prefiltered map

- 우선 Light part는 Pre-filtered environment map을 사용한다. Pre-filtered map에는 물체에 비칠 주변 환경의 형상을 저장하는데, roughness에 따라 반사되는 정도가 달라지므로 (roughness가 커질 수록 빛이 분산되는 정도가 커져 흐리게 보인다.) roughness가 클 수록 mipmap level을 높여 더 흐려보이게 된다.
- 이 map을 구하는데에는 통계적인 적분 방식인 몬테 카를로 적분 방식을 사용한다. 이 방식에서는 랜덤한 위치에서 샘플을 가져온 뒤 그 샘플에서 구한 값을 전체 합계에 누적시키는 방식을 사용해서 근사한 적분값을 계산해 낸다. 여기서는 샘플의 위치를 구할 때 약간 더 분산되도록 하여(low-discrepancy sequence) 조금 더 빠르게 적분값에 수렴할 수 있는 Quasi-Monte Carlo integration 방법을 사용한다.
- texture 파일에서는 cubemap에서 mipmap을 만들어 낼 수 있도록 약간의 코드를 추가했다.
- Prefiltered map을 생성하는 prefiltered_light.frag 쉐이더를 만들었다. 이 쉐이더에서는 1024개의 샘플을 임의의 위치에서 들고온 뒤, 해당 위치에서의 반사 벡터를 구하고 그 반사 벡터와 법선 벡터의 내적이 0이상이면 색상의 합에 누적시키는 방식을 이용해서 색상을 구하고 있다.

```glsl
// reflection, view 방향을 normal로 통일
vec3 N = normalize(localPos);
vec3 R = N;
vec3 V = R;

// 샘플 1024개로 통계적 계산
const uint SAMPLE_COUNT = 1024u;
float totalWeight = 0.0;
vec3 prefilteredColor = vec3(0.0);
for(uint i = 0u; i < SAMPLE_COUNT; ++i) {
    // 랜덤 포인트 생성
    vec2 Xi = Hammersley(i, SAMPLE_COUNT);
    // 해당하는 면에서 반사되는 방향 벡터를 도출
    vec3 H = ImportanceSampleGGX(Xi, N, roughness);
    vec3 L = normalize(2.0 * dot(V, H) * H - V);
    float NdotL = max(dot(N, L), 0.0);
    if(NdotL > 0.0) {
        prefilteredColor += texture(cubeMap, L).rgb * NdotL;
        totalWeight += NdotL;
    }
}
prefilteredColor = prefilteredColor / totalWeight;

fragColor = vec4(prefilteredColor, 1.0);
```

- context 파일에서 맴버를 추가하고, 초기화하는 부분을 넣어 준다. roughness에 따라 5단계의 밉맵을 생성하고, 그 밉맵으로 부터 6면의 텍스쳐를 생성하는 방식으로 했다.

```c++
const uint32_t maxMipLevels = 5;
glDepthFunc(GL_LEQUAL);
m_preFilteredProgram = Program::Create("./shader/skybox_hdr.vert", "./shader/prefiltered_light.frag");
m_preFilteredMap = CubeTexture::Create(128, 128, GL_RGB16F, GL_FLOAT);
m_preFilteredMap->GenerateMipmap();
m_preFilteredProgram->Use();
m_preFilteredProgram->SetUniform("projection", projection);
m_preFilteredProgram->SetUniform("cubeMap", 0);
m_hdrCubeMap->Bind();
for (uint32_t mip = 0; mip < maxMipLevels; mip++) {
    auto framebuffer = CubeFramebuffer::Create(m_preFilteredMap, mip);
    uint32_t mipWidth = 128 >> mip;
    uint32_t mipHeight = 128 >> mip;
    glViewport(0, 0, mipWidth, mipHeight);

    float roughness = (float)mip / (float)(maxMipLevels - 1);
    m_preFilteredProgram->SetUniform("roughness", roughness);
    for (uint32_t i = 0; i < (int)views.size(); i++) {
        m_preFilteredProgram->SetUniform("view", views[i]);
        framebuffer->Bind(i);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_box->Draw(m_preFilteredProgram.get());   
    }
}
glDepthFunc(GL_LESS);
```

- 모든 mipmap의 결과를 확인하기 위해 skybox_hdr.frag에서 roughness를 uniform으로 받고, textureLod 함수를 사용해 밉맵 레벨에 따라 텍스쳐에서 픽셀값을 얻어오도록 수정했다.

```glsl
uniform float roughness;
// main 함수
vec3 envColor = textureLod(cubeMap, localPos, roughness * 4).rgb;
```

- 실행된 모습
![PBR18](/note_image/PBR18.png)
- Roughness 수치가 높아질 수록 더 높은 레벨의 mipmap으로 그려지기 때문에 점점 흐려지는 것을 확인할 수 있다.
- 그러나 이렇게 만든 Prefiltered map에는 2가지 문제가 있다. 첫 번째 문제는 roughness가 0.2 ~ 0.5쯤 될 때 점박 형태의 무늬가 나타나는 것이다.
![PBR19-1](/note_image/PBR19-1.png)
- 두 번째 문제는 roughness가 클 때(0.5 이상) 큐브맵의 경계선이 뚜렷하게 보인다는 것이다.
![PBR19-2](/note_image/PBR19-2.png)

- 큐브맵의 경계선이 보이는 문제는 Init() 함수 맨 위에 glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS)를 추가하는 것으로 해결할 수 있다.
- 점박 무늬가 보이는 문제는 roughness 수치에 따라 textureLod 함수를 이용해 서로 다른 mipmap에서 샘플링을 하는 것으로 해결할 수 있다. 새로운 함수 DistributionGGX를 추가하고 main 함수 내에 if문을 아래와 같이 수정한다.

```glsl
float D = DistributionGGX(N, H, roughness);
float NdotH = max(dot(N, H), 0.0);
float HdotV = max(dot(H, V), 0.0);
float pdf = (D * NdotH / (4.0 * HdotV)) + 0.0001;
float resolution = 512.0; // resolution of source cubemap (per face)
float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);
float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

// prefilteredColor += texture(cubeMap, L).rgb * NdotL;
prefilteredColor += textureLod(cubeMap, L, mipLevel).rgb * NdotL;
totalWeight += NdotL;
```

- 이렇게 한 뒤 실행하면 경계선과 점박 무늬가 사라졌음을 확인할 수 있다.

##### BRDF lookup table

- 이제 두 번째 부분인 BRDF 적분식을 작성할 때이다.
- BRDF 적분식의 계산 결과를 저장하는 텍스쳐 맵을 생성하는 새로운 쉐이더 brdf_lookup를 만든다.
- vertex shader에서는 위치와 텍스쳐 좌표를 받아 변환된 좌표를 전달하는 간단한 역할을 하고, fragment shader에서는 Prefiltered map과 비슷하게 임의의 지점을 선택한 뒤 그 지점에서의 BRDF 적분 값을 구해서 합하는 방식으로 구한다.

```glsl
vec3 V;
V.x = sqrt(1.0 - NdotV*NdotV);
V.y = 0.0;
V.z = NdotV;

float A = 0.0;
float B = 0.0;

vec3 N = vec3(0.0, 0.0, 1.0);

const uint SAMPLE_COUNT = 1024u;
for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
    vec2 Xi = Hammersley(i, SAMPLE_COUNT);
    vec3 H = ImportanceSampleGGX(Xi, N, roughness);
    vec3 L = normalize(2.0 * dot(V, H) * H - V);

    float NdotL = max(L.z, 0.0);
    float NdotH = max(H.z, 0.0);
    float VdotH = max(dot(V, H), 0.0);

    if(NdotL > 0.0) {
        float G = GeometrySmith(N, V, L, roughness);
        float G_Vis = (G * VdotH) / (NdotH * NdotV);
        float Fc = pow(1.0 - VdotH, 5.0);
        A += (1.0 - Fc) * G_Vis;
        B += Fc * G_Vis;
    }
}
A /= float(SAMPLE_COUNT);
B /= float(SAMPLE_COUNT);
return vec2(A, B);
```

- Init 함수에서 BRDF lookup table을 프로그램을 사용해서 만들어 준다.

```c++
m_brdfLookupProgram = Program::Create("./shader/brdf_lookup.vert", "./shader/brdf_lookup.frag");
m_brdfLookupMap = Texture::Create(512, 512, GL_RG16F, GL_FLOAT);
auto lookupFramebuffer = Framebuffer::Create({ m_brdfLookupMap });
lookupFramebuffer->Bind();
glViewport(0, 0, 512, 512);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
m_brdfLookupProgram->Use();
m_brdfLookupProgram->SetUniform("transform",
glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, -2.0f, 2.0f)));
m_plane->Draw(m_brdfLookupProgram.get());
```

- 이렇게 만들어진 맵을 ImGUI로 만들어내면 아래와 같은 모습이 나온다.
![PBR20](/note_image/PBR20.png)

- 마지막으로 이렇게 만들어진 BRDF를 적용해주면 끝난다.
- pbr.frag에서 useIBL이 1이면, 이전에 만들었던 irradiance map과 perfiltered map, BRDF map를 적용해서 최종적인 색을 구해주는 부분을 추가한다.

```glsl
// ao 맵으로 부터 주변광 색을 가져옴
vec3 ambient = vec3(0.03) * albedo * ao;
// IBL 사용
if (useIBL == 1) {
    vec3 kS = FresnelSchlickRoughness(dotNV, F0, roughness);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    // diffuse part - irradiance map
    vec3 irradiance = texture(irradianceMap, fragNormal).rgb;
    vec3 diffuse = irradiance * albedo;

    // specular part
    vec3 R = reflect(-viewDir, fragNormal);
    const float MAX_REFLECTION_LOD = 4.0;
    // perfiltered map
    vec3 preFilteredColor = textureLod(preFilteredMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    // BRDF map
    vec2 envBrdf = texture(brdfLookupTable, vec2(dotNV, roughness)).rg;
    vec3 specular = preFilteredColor * (kS * envBrdf.x + envBrdf.y);

    ambient = (kD * diffuse + specular) * ao;
}
vec3 color = ambient + outRadiance;
```

- 이후 skybox_hdr.frag shader를 다시 되돌리고 uniform 변수의 값으로 만들어진 map 들을 넘겨주는 코드를 써주면 된다.
- 실행된 결과
![PBR21-1](/note_image/PBR21-1.png)
![PBR21-2](/note_image/PBR21-2.png)

- 그려진 구들 중에서 roughness = 0, metallic = 1인 구를 가까이에서 보면 IBL에 의해 그려진 주변 배경의 모습이 구에 비추어 지는 것을 확인할 수 있다.
![PBR22](/note_image/PBR22.png)

### OpenGL 오류 처리 방법

- OpenGL에서는 gl계열 함수에서 문제가 발생했는지 여부를 glGetError()를 호출함으로써 알 수 있다. 이 함수는 바로 전의 gl 함수 호출의 결과를 GLenum의 형태로 반환한다. 오류가 나지 않은 경우 GL_NO_ERROR, 아닌 경우 아래 표에 있는 GLenum 중 1개를 반환한다.

| GLenum | 설명 |
|--|--|
| GL_NO_ERROR | 오류가 발생하지 않았다. 이 상수의 값은 0으로 보장된다. |
| GL_INVALID_ENUM | 열거형 인수에 대해 허용되지 않는 값이 지정되었을 때 발생한다. 오류 플래그를 설정하는 것 외에는 부작용이 없다. |
| GL_INVALID_VALUE | 숫자 인수가 범위를 벗어날 때 발생한. 오류 플래그를 설정하는 것 외에는 부작용이 없다. |
| GL_INVALID_OPERATION | 현재 상태에서 허용되지 않은 작업을 했을 때 발생한다. 오류 플래그를 설정하는 것 외에는 부작용이 없다. |
| GL_INVALID_FRAMEBUFFER_OPERATION | 프레임 버퍼 객체가 완전하지 않을 때 발생한다. 오류 플래그를 설정하는 것 외에는 부작용이 없다. |
| GL_OUT_OF_MEMORY | 함수를 실행할 메모리가 부족할 때 발생한다. 이 오류가 기록된 후 오류 플래그의 상태를 제외하고 OpenGL의 상태는 정의되지 않는다. |
| GL_STACK_UNDERFLOW | 함수에서 스택 언더플로가 발생했을 때 반환된다. 오류 플래그를 설정하는 것 외에는 부작용이 없다. |
| GL_STACK_OVERFLOW | 함수에서 스택 오버플로가 발생했을 때 반환된다. 오류 플래그를 설정하는 것 외에는 부작용이 없다. |

- glGetError를 사용할 때 주의할 점이 하나 있는데, glGetError가 오류를 찾아내서 오류 플래그를 설정했을 때, 플래그를 초기화 하지 않으면 다른 모든 오류는 무시한다는 점이다. 그래서 이 함수를 사용해서 오류 코드(GL_NO_ERROR 포함)를 얻었다면 플래그를 초기화 해서 다른 오류를 잡을 준비를 해야 한다.

```c++
// 오류 플래그를 재설정하는 함수
static void GLClearError() 
{
    while (glGetError() != GL_NO_ERROR);
}

// 오류 체크, 원인 출력 함수
static bool GLLogCall()
{
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ")" << std::endl;
        return false;
    }
    return true;
}
```

- 위의 두 함수를 gl 함수 위와 아래에 배치함으로써 오류 검사를 할 수 있다.

```c++
GLClearError();
glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr);
GLLogCall();
```

- 하지만 더 두 함수를 모든 gl 함수의 위/아래마다 붙이는 것은 쉽지 않으므로 아래와 같이 매크로를 사용하면 더 손쉽게 쓸 수 있다. (Note : __debugbreak()는 WIN32 API에 있는 함수로 windows에서만 쓸 수 있다.)

```c++
#define ASSERT(x) if(!(x)) __debugbreak();
#ifdef DEBUG
   #define GLCall( x ) \
     GLClearError(); \
     x; \
     ASSERT( GLLogCall( #x, __FILE__, __LINE__) ) __debugbreak();
   #define GLCallR( x ) [&]() { \
     GLClearError(); \
     auto retVal = x; \
     ASSERT( GLLogCall( #x, __FILE__, __LINE__) ) __debugbreak(); \
     return retVal; \
   }()
#else
   #define GLCall( x ) x
   #define GLCallR( x ) x
#endif

// 일부 변형함
// Source : https://www.youtube.com/watch?v=FBbPWSOQ0-w&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&index=10
```

- GLCall 매크로는 일반적인 gl 함수에, GLCallR 매크로는 gl 함수 중에서 반환값이 있고, 그 값을 특정 변수에 저장하는 경우에 사용한다.

```c++
GLCall(glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr));
unsigned int id = GLCallR(glCreateShader(type));
```

- GLLogCall 함수를 수정하여 오류의 종류, 오류가 난 곳의 파일과 줄을 더 친절하게 설명할 수 있도록 한다.

```c++
// 오류를 감지하고 오류 코드를 출력하는 함수
static bool GLLogCall(const char* function, const char* file, int line) 
{
    while (GLenum error = glGetError()) {
        // 오류 발생시 오류 코드에 해당하는 오류의 종류를 판단한다.
        std::string errStr;
        switch (error)
        {
            case GL_INVALID_ENUM:                  errStr = "Invalid enum error"; break;
            case GL_INVALID_VALUE:                 errStr = "Invalid value error"; break;
            case GL_INVALID_OPERATION:             errStr = "Invalid operation error"; break;
            case GL_STACK_OVERFLOW:                errStr = "Stack Overflow"; break;
            case GL_STACK_UNDERFLOW:               errStr = "Stack Underflow"; break;
            case GL_OUT_OF_MEMORY:                 errStr = "Out of memory"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: errStr = "Invalid framebuffer operation error"; break;
            default:                               errStr = "Unknown error"; break;
        }
        // 오류 메시지를 출력한다.
        std::cout << "[OpenGL Error] " << errStr <<  " (" << error << ")" << std::endl;
        std::cout << "Happened in File " << file << " in line " << line << std::endl;
        std::cout << function << std::endl;
        return false;
    }
    return true;
}
// 참조 : https://learnopengl.com/In-Practice/Debugging
```
