- 이 코드는 아래 유튜브에 있는 강원대학교 2021-1학기 그래픽스 강의 수업을 들으며 따라한 코드를 올려 놓은 곳입니다.
- <https://www.youtube.com/playlist?list=PLvNHCGtd4kh_cYLKMP_E-jwF3YKpDP4hf>

- 좋은 수업과 자료를 모든 사람을 위해 공개해 주신 권지용 교수님께 감사드립니다.
- 텍스쳐와 3D 모델은 원본 튜토리얼인 <https://learnopengl.com/> 에서 가져온 것입니다.
- Dependency.cmake 파일은 windows 운영체제를 기준으로 작성되었고, 다른 운영체제에서는 그대로 사용시 실행되지 않을 수도 있습니다.
- 현재 8-2 코드에는 오타가 있습니다. 실행은 되지만 빛이 이상하게 동작할 수 있는데, 8-2 commit에 어떤 오타가 있는지 메모가 있으므로 참고해서 고치면 문제 없을 것입니다.

## 강의 내용 정리 노트 - <https://rinthel.github.io/opengl_course/> 참고

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
- 행동에는 GL_KEEP (스텐실 값 유지), GL_ZERO (0으로 만듦), GL_REPLACE (glStencilFunc에서 지정한 2번째 인자의 값으로 바꿈), GL_INVERT (스텐실 값에 bit not 연산 수행) 등이 있다. (https://heinleinsgame.tistory.com/25 참고)

### blending

- 그리려고 하는 색상과 프레임버퍼에 이미 입력되어 있는 색상끼리 연산해서 새로운 색상을 만들어 내는 것
- 주로 반투명한 오브젝트를 그리고, 그 오브젝트와 뒤에 있는 오브젝트끼리 만나서 색이 합성되는 효과를 내기 위해 사용된다.
- 블랜딩을 통한 최종 색깔은 아래 공식에 의해 결정된다.
$\vec{C_{result}}=\vec{C_{source}}*F_{source}+\vec{C_{dest}}*F_{dest}$
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

```GLSL
vec3 I = normalize(position - cameraPos);
vec3 R = reflect(I, normalize(normal));
fragColor = vec4(texture(skybox, R).rgb, 1.0);
```

- 단독으로 사용할 경우 SkyBox의 모습이 그대로 물체에 그려지므로, 그 물체 주변에 다른 물체들이 있더라도 무시되고 오로지 SkyBox의 모습만 그려져 비현실적일 수 있다. 그래서 빛을 렌더링하는 일반적인 쉐이더들과 함께 사용한다.
