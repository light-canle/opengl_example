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
