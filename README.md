- 이 코드는 아래 유튜브에 있는 강원대학교 2021-1학기 그래픽스 강의 수업을 들으며 따라한 코드를 올려 놓은 곳입니다.
- https://www.youtube.com/playlist?list=PLvNHCGtd4kh_cYLKMP_E-jwF3YKpDP4hf

- 좋은 수업과 자료를 모든 사람을 위해 공개해 주신 권지용 교수님께 감사드립니다.
- 텍스쳐와 3D 모델은 원본 튜토리얼인 https://learnopengl.com/ 에서 가져온 것입니다.
- Dependency.cmake 파일은 windows 운영체제를 기준으로 작성되었고, 다른 운영체제에서는 그대로 사용시 실행되지 않을 수도 있습니다.
- 현재 8-2 코드에는 오타가 있습니다. 실행은 되지만 빛이 이상하게 동작할 수 있는데, 8-2 commit에 어떤 오타가 있는지 메모가 있으므로 참고해서 고치면 문제 없을 것입니다.

### 3D 관련 간단 노트

- Note : 확대 -> 회전 -> 평행이동 순으로 점에 선형 변환 적용
- translate로 평행이동, rotate로 회전, scale로 크기 변경

- Uniform 변수 값 지정 - 프로그램 생성 이후에 배치
  
```c++
// 프로그램에서 color uniform 변수의 위치를 가져옴
auto loc = glGetUniformLocation(m_program->Get(), "color");
// 해당 위치에 값을 넣음(color는 vec4이다.)
glUniform4f(loc, 1.0f, 1.0f, 0.0f, 1.0f);
```

- lookAt 함수는 아래와 같이 카메라 행렬의 역행렬을 계산해준다.

```c++
auto cameraZ = glm::normalize(cameraPos - cameraTarget); // 카메라 z방향은 e - t 벡터의 방향이다.
auto cameraX = glm::normalize(glm::cross(cameraUp, cameraZ)); // 카메라 x 방향은 up 벡터와 z방향의 외적이다.
auto cameraY = glm::cross(cameraZ, cameraX); // 카메라 y방향은 z방향과 x방향의 외적이다.
```

- 구한 벡터들로 카메라 행렬(view matrix)을 구함

```c++
auto cameraMat = glm::mat4(
    glm::vec4(cameraX, 0.0f),
    glm::vec4(cameraY, 0.0f),
    glm::vec4(cameraZ, 0.0f),
    glm::vec4(cameraPos, 1.0f));

auto view = glm::inverse(cameraMat); // view 행렬은 카메라 행렬의 역행렬이다.
```

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
