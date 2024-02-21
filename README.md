# OpenGL Example

- 이 코드는 아래 유튜브에 있는 강원대학교 2021-1학기 그래픽스 강의 수업을 들으며 따라한 코드를 올려 놓은 곳입니다.
- <https://www.youtube.com/playlist?list=PLvNHCGtd4kh_cYLKMP_E-jwF3YKpDP4hf>

- 좋은 수업과 자료를 모든 사람을 위해 공개해 주신 권지용 교수님께 감사드립니다.
- 텍스쳐와 3D 모델은 원본 튜토리얼인 <https://learnopengl.com> 에서 가져온 것입니다.
- PBR 텍스쳐는 <https://freepbr.com/materials/rusted-iron-pbr-metal-material-alt/> 에서 가져왔습니다.
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

#### PBR 구현 (https://www.youtube.com/watch?v=HuGEcaMpqEQ&list=PLvNHCGtd4kh_cYLKMP_E-jwF3YKpDP4hf&index=42))

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
