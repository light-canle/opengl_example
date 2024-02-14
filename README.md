# OpenGL Example

- 이 코드는 아래 유튜브에 있는 강원대학교 2021-1학기 그래픽스 강의 수업을 들으며 따라한 코드를 올려 놓은 곳입니다.
- <https://www.youtube.com/playlist?list=PLvNHCGtd4kh_cYLKMP_E-jwF3YKpDP4hf>

- 좋은 수업과 자료를 모든 사람을 위해 공개해 주신 권지용 교수님께 감사드립니다.
- 텍스쳐와 3D 모델은 원본 튜토리얼인 <https://learnopengl.com> 에서 가져온 것입니다.
- Dependency.cmake 파일은 windows 운영체제를 기준으로 작성되었고, 다른 운영체제에서는 그대로 사용시 실행되지 않을 수도 있습니다.
- 사용된 라이브러리들은 그 라이브러리의 최신 버전이 아닌 강의의 버전과 동일한 것을 사용했습니다. 최신 버전의 라이브러리에서는 이 코드가 동작하지 않을 수도 있습니다.
- 현재 8-2 코드에는 오타가 있습니다. 실행은 되지만 빛이 이상하게 동작할 수 있는데, 8-2 commit에 어떤 오타가 있는지 메모가 있으므로 참고해서 고치면 문제 없을 것입니다.

## 강의 내용 정리 노트 - <https://rinthel.github.io/opengl_course> 참고

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

- Note : HDR과 Bloom 부분은 원 강의에서 예제 작성 없이 learnopengl.com의 코드만 설명했으므로 여기서도 소스 코드 작성은 없이 여기에 learnopengl.com의 내용을 가져왔다. 소스 코드는 제목에 있는 사이트에서 가져왔다.

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

1. Tone mapping
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

2. Exposure tone mapping
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
