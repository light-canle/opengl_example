#include "mesh.h"

MeshUPtr Mesh::Create(
    const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices,
    uint32_t primitiveType) {
    auto mesh = MeshUPtr(new Mesh());
    mesh->Init(vertices, indices, primitiveType);
    return std::move(mesh);
}

void Mesh::Init(
    const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices,
    uint32_t primitiveType) {
    // 0. 만약 primitive가 삼각형인 경우 자동으로 접선 벡터를 계산
    // const_cast는 Init 함수의 인자 vertices에 있는 const를 떼어내기 위해 쓴다.
    if (primitiveType == GL_TRIANGLES) {
        ComputeTangents(const_cast<std::vector<Vertex>&>(vertices), indices);
    }
    // 1. VAO 바인딩 - VBO 바인딩 전에 함(그래야 VAO에 대응하는 VBO가 지정된다.)
    m_vertexLayout = VertexLayout::Create();
    // 2. VBO 바인딩 / 정점 데이터 복사
    // 마지막 인자는 정점 리스트의 데이터 크기이다.
    m_vertexBuffer = Buffer::CreateWithData(
        GL_ARRAY_BUFFER, GL_STATIC_DRAW,
        vertices.data(), sizeof(Vertex), vertices.size());

    // 3. EBO 바인딩
    // 5번째 인자에 곱해지는 수는 인덱스 배열의 길이이다.
    m_indexBuffer = Buffer::CreateWithData(
        GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
        indices.data(), sizeof(uint32_t), indices.size());

    // 4. VAO에서 정점 데이터의 속성을 지정
    // 5번째 인자에 곱해지는 값은 한 정점 당 데이터 크기이다.(지금은 위치 3개, 법선 3개, 텍스쳐 2개, 접선 3개로 총 11개)
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(Vertex), 0); // 위치(0~2)
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, normal)); // 법선 벡터(3~5)
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, texCoord)); // 텍스쳐 좌표(6~7)
    m_vertexLayout->SetAttrib(3, 3, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, tangent)); // 접선 벡터(8~10)
}

void Mesh::Draw(const Program* program) const {
    // VAO 바인딩
    m_vertexLayout->Bind();
    // material이 있으면 program에 세팅
    if (m_material) {
        m_material->SetToProgram(program);
    }
    // 그림을 그림
    glDrawElements(m_primitiveType, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
}

// 기본적인 박스 형태로 매쉬를 초기화
MeshUPtr Mesh::MakeBox() {
    std::vector<Vertex> vertices = {
        Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f) },

        Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f) },

        Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

        Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

        Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

        Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
    };

    std::vector<uint32_t> indices = {
        0,  2,  1,  2,  0,  3,
        4,  5,  6,  6,  7,  4,
        8,  9, 10, 10, 11,  8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23,
    };

    return Mesh::Create(vertices, indices, GL_TRIANGLES);
}

MeshUPtr Mesh::CreatePlane() {
    std::vector<Vertex> vertices = {
        Vertex { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
    };

    std::vector<uint32_t> indices = {
        0,  1,  2,  2,  3,  0,
    };

    return Create(vertices, indices, GL_TRIANGLES);
}

void Material::SetToProgram(const Program* program) const {
    int textureCount = 0;
    // diffuse 맵이 있는 경우
    if (diffuse) {
        glActiveTexture(GL_TEXTURE0 + textureCount);
        program->SetUniform("material.diffuse", textureCount);
        diffuse->Bind();
        textureCount++;
    }
    // specular 맵이 있는 경우
    if (specular) {
        glActiveTexture(GL_TEXTURE0 + textureCount);
        program->SetUniform("material.specular", textureCount);
        specular->Bind();
        textureCount++;
    }
    glActiveTexture(GL_TEXTURE0);
    program->SetUniform("material.shininess", shininess);
}

void Mesh::ComputeTangents(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {

    auto compute = [](
        // 삼각형을 이루는 세 정점의 위치
        const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3,
        // 세 정점의 텍스쳐 좌표
        const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3)
        -> glm::vec3 {

        auto edge1 = pos2 - pos1;
        auto edge2 = pos3 - pos1;
        auto deltaUV1 = uv2 - uv1;
        auto deltaUV2 = uv3 - uv1;
        float det = (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        if (det != 0.0f) {
            auto invDet = 1.0f / det;
            return invDet * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
        }
        else {
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }
    };

    // initialize
    std::vector<glm::vec3> tangents;
    tangents.resize(vertices.size());
    memset(tangents.data(), 0, tangents.size() * sizeof(glm::vec3));

    // accumulate triangle tangents to each vertex - 모든 vertex에 대해 평면 삼각형에 대한 tangent vector를 계산
    for (size_t i = 0; i < indices.size(); i += 3) {
        auto v1 = indices[i  ];
        auto v2 = indices[i+1];
        auto v3 = indices[i+2];

        tangents[v1] += compute(
            vertices[v1].position, vertices[v2].position, vertices[v3].position,
            vertices[v1].texCoord, vertices[v2].texCoord, vertices[v3].texCoord);

        tangents[v2] = compute(
            vertices[v2].position, vertices[v3].position, vertices[v1].position,
            vertices[v2].texCoord, vertices[v3].texCoord, vertices[v1].texCoord);

        tangents[v3] = compute(
            vertices[v3].position, vertices[v1].position, vertices[v2].position,
            vertices[v3].texCoord, vertices[v1].texCoord, vertices[v2].texCoord);
    }

    // normalize
    for (size_t i = 0; i < vertices.size(); i++) {
        vertices[i].tangent = glm::normalize(tangents[i]);
    }
}

MeshUPtr Mesh::CreateSphere(uint32_t latiSegmentCount, uint32_t longiSegmentCount) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    uint32_t circleVertCount = longiSegmentCount + 1;
    vertices.resize((latiSegmentCount + 1) * circleVertCount);
    // 위도와 경도가 주어질 때 점의 위치를 구면 좌표계에서 찾는다.
    for (uint32_t i = 0; i <= latiSegmentCount; i++) {
        float v = (float)i / (float)latiSegmentCount;
        float phi = (v - 0.5f) * glm::pi<float>();
        auto cosPhi = cosf(phi);
        auto sinPhi = sinf(phi);
        for (uint32_t j = 0; j <= longiSegmentCount; j++) {
            float u = (float)j / (float)longiSegmentCount;
            float theta = u * glm::pi<float>() * 2.0f;
            auto cosTheta = cosf(theta);
            auto sinTheta = sinf(theta);
            auto point = glm::vec3(
                cosPhi * cosTheta, sinPhi, -cosPhi * sinTheta);
        
            vertices[i * circleVertCount + j] = Vertex {
                point * 0.5f, point, glm::vec2(u, v), glm::vec3(0.0f)
            };
        }
    }

    indices.resize(latiSegmentCount * longiSegmentCount * 6);
    for (uint32_t i = 0; i < latiSegmentCount; i++) {
        for (uint32_t j = 0; j < longiSegmentCount; j++) {
            uint32_t vertexOffset = i * circleVertCount + j;
            uint32_t indexOffset = (i * longiSegmentCount + j) * 6;
            indices[indexOffset    ] = vertexOffset;
            indices[indexOffset + 1] = vertexOffset + 1;
            indices[indexOffset + 2] = vertexOffset + 1 + circleVertCount;
            indices[indexOffset + 3] = vertexOffset;
            indices[indexOffset + 4] = vertexOffset + 1 + circleVertCount;
            indices[indexOffset + 5] = vertexOffset + circleVertCount;
        }
    }

    return Create(vertices, indices, GL_TRIANGLES);
}