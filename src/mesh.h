#ifndef __MESH_H__
#define __MESH_H__

#include "common.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "program.h"

// 1개의 vertex에 대한 정보를 담은 struct
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

CLASS_PTR(Material);
class Material {
public:
    static MaterialUPtr Create() {
        return MaterialUPtr(new Material());
    }
    TexturePtr diffuse;
    TexturePtr specular;
    float shininess { 32.0f };

    void SetToProgram(const Program* program) const;

private:
    Material() {}
};

CLASS_PTR(Mesh);
class Mesh {
public:
    static MeshUPtr Create(
        const std::vector<Vertex>& vertices, // 정점 리스트
        const std::vector<uint32_t>& indices, // 인덱스 데이터
        uint32_t primitiveType); // primitive type
    // box를 생성하는 함수
    static MeshUPtr MakeBox();

    const VertexLayout* GetVertexLayout() const { return m_vertexLayout.get(); }
    BufferPtr GetVertexBuffer() const { return m_vertexBuffer; }
    BufferPtr GetIndexBuffer() const { return m_indexBuffer; }
    
    // Material을 설정하고 가져오는 함수
    void SetMaterial(MaterialPtr material) { m_material = material; }
    MaterialPtr GetMaterial() const { return m_material; }

    // 해당 데이터의 물체를 렌더링
    void Draw(const Program* program) const;

private:
    Mesh() {}
    void Init(
        const std::vector<Vertex>& vertices,
        const std::vector<uint32_t>& indices,
        uint32_t primitiveType);

    // primitive Type : 어떤 모양으로 그릴 것인가?
    uint32_t m_primitiveType { GL_TRIANGLES };
    // 정점 레이아웃 (정점 데이터 구성 정보 - 위치, 텍스쳐 좌표 등)
    VertexLayoutUPtr m_vertexLayout;
    // VBO
    BufferPtr m_vertexBuffer;
    // EBO
    BufferPtr m_indexBuffer;
    // Material
    MaterialPtr m_material;
};

#endif // __MESH_H__