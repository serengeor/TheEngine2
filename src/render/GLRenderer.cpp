#include "GLRenderer.h"
#include "GLFrameBufferObject.h"
#include "GLGpuBufferArrayObject.h"
#include "GLGpuBufferObject.h"
#include "GLGpuShaderProgram.h"
#include "GLRenderBufferObject.h"
#include "GLRendererDebugMessageMonitor.h"
#include "GLTexture.h"
#include "OpenGL.hpp"
#include "RenderContext.h"
#include "render/BaseMesh.h"
#include "render/AnimatedMesh.h"
#include "render/CTexture.h"
#include "render/ICamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace render {
core::UniquePtr<IRenderer> CreateRenderer(
    core::UniquePtr<GLRendererDebugMessageMonitor>&& debugMessageMonitor)
{
    return core::MakeUnique<GLRenderer>(core::Move(debugMessageMonitor));
}

GLRenderer::GLRenderer(core::UniquePtr<GLRendererDebugMessageMonitor>&& debugMessageMonitor)
    : m_debugMessageMonitor(core::Move(debugMessageMonitor))
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    m_renderContext = core::MakeUnique<RenderContext>(this);
}
GLRenderer::~GLRenderer()
{
}

IRendererDebugMessageMonitor* GLRenderer::GetDebugMessageMonitor()
{
    return m_debugMessageMonitor.get();
}

core::SharedPtr<IGpuProgram> GLRenderer::CreateProgram(const core::String& vertSource,
                                                       const core::String& fragSource,
                                                       const core::String& geomSource)
{
    auto handle = gl::CreatePipelineFromShaderStrings(vertSource.c_str(), fragSource.c_str(),
                                                      geomSource.c_str());

    if (gl::IsHandleValid(handle))
        return core::MakeShared<GLGpuShaderProgram>(handle);
    else
        return nullptr;
}

core::SharedPtr<IGpuBufferArrayObject> GLRenderer::CreateBufferArrayObject(
    const core::Vector<BufferDescriptor>& descriptors)
{
    // setup buffers
    auto handles = gl::CreateGpuStorages(descriptors.size());
    auto IsValid = [](const gl::gpu_buffer_object_handle& h) { return gl::IsHandleValid(h); };

    if (core::Any(handles, IsValid) == false)
        return nullptr;

    core::Vector<core::UniquePtr<IGpuBufferObject>> buffers;
    for (uint32_t i = 0; i < handles.size(); i++) {
        gl::ProcessHandle(descriptors[i], handles[i]);
        buffers.push_back(core::MakeUnique<GLGpuBufferObject>(handles[i]));
    }

    // create the vao
    auto handle = gl::CreateVertexArrayObject();
    if (gl::IsHandleValid(handle) == false)
        return nullptr;

    auto vao = core::MakeShared<GLGpuBufferArrayObject>(handle, core::Move(buffers));

    vao->EnableBuffers();

    return vao;
}

core::SharedPtr<ITexture> GLRenderer::CreateTexture(const TextureDescriptor& descriptor)
{
    return GLTexture::CreateTexture(descriptor);
}

core::SharedPtr<IFrameBufferObject> GLRenderer::CreateFrameBufferObject(
    const FrameBufferObjectDescriptor& descriptor)
{
    return GLFrameBufferObject::CreateFrameBufferObject(descriptor);
}


core::SharedPtr<IRenderBufferObject> GLRenderer::CreateRenderBufferObject(
    const RenderBufferObjectDescriptor& descriptor)
{
    return GLRenderBufferObject::Create(descriptor);
}

void GLRenderer::SetActiveTextures(const core::Array<ITexture*,8>& textures)
{
    int slot = 0;
    for (int32_t i = 0; i < textures.size(); i++) {
        if(textures[i]){
            auto texture = static_cast<GLTexture*>(textures[i]);
            GLTexture::BindObject(texture, slot);
            slot++;
        }
    }
}

void GLRenderer::SetActiveFrameBuffer(core::SharedPtr<IFrameBufferObject> fbo,
                                      FrameBufferTarget target)
{
    m_activeFrameBufferObject = std::static_pointer_cast<GLFrameBufferObject>(fbo);
    GLFrameBufferObject::BindObject(m_activeFrameBufferObject.get(), target);
}

void GLRenderer::SetClearColor(const Vec3i& color)
{
    gl::SetClearColor(color);
}

void GLRenderer::Clear()
{
    gl::Clear();
}

core::UniquePtr<BaseMesh> GLRenderer::CreateBaseMesh()
{
    core::Vector<render::BufferDescriptor> bufferDescriptors = {
        render::BufferDescriptor{ 1, render::BufferObjectType::index,
                                  render::BufferComponentDataType::uint32, 0 },

        render::BufferDescriptor{ 2, render::BufferObjectType::vertex,
                                  render::BufferComponentDataType::float32, 0 },

        render::BufferDescriptor{ 3, render::BufferObjectType::vertex,
                                  render::BufferComponentDataType::float32, 1 },

        render::BufferDescriptor{ 3, render::BufferObjectType::vertex,
                                  render::BufferComponentDataType::float32, 2 },

        render::BufferDescriptor{ 3, render::BufferObjectType::vertex,
                                  render::BufferComponentDataType::float32, 3 }
    };

    auto vao      = this->CreateBufferArrayObject(bufferDescriptors);
    auto baseMesh = core::MakeUnique<BaseMesh>(vao);
    return baseMesh;
}

core::UniquePtr<AnimatedMesh> GLRenderer::CreateAnimatedMesh()
{
    core::Vector<render::BufferDescriptor> bufferDescriptors = {
        render::BufferDescriptor{ 1, render::BufferObjectType::index,
                                  render::BufferComponentDataType::uint32, 0 },

        render::BufferDescriptor{ 2, render::BufferObjectType::vertex,
                                  render::BufferComponentDataType::float32, 0 },

        render::BufferDescriptor{ 3, render::BufferObjectType::vertex,
                                  render::BufferComponentDataType::float32, 1 },

        render::BufferDescriptor{ 3, render::BufferObjectType::vertex,
                                  render::BufferComponentDataType::float32, 2 },

        render::BufferDescriptor{ 4, render::BufferObjectType::vertex,
                                  render::BufferComponentDataType::uint8, 3 },

        render::BufferDescriptor{ 4, render::BufferObjectType::vertex,
                                  render::BufferComponentDataType::uint8, 4 }
    };

    auto vao      = this->CreateBufferArrayObject(bufferDescriptors);
    auto baseMesh = core::MakeUnique<AnimatedMesh>(vao);
    return baseMesh;
}

void GLRenderer::BeginFrame()
{
    this->Clear();
}

void GLRenderer::EndFrame()
{
}

void GLRenderer::RenderMesh(BaseMesh * mesh, material::BaseMaterial * material, const glm::vec3 position){
    RenderMesh(mesh, material, glm::translate(glm::mat4(1), position));
}

void GLRenderer::RenderMesh(BaseMesh * mesh, material::BaseMaterial * material, const glm::mat4 transform){
    auto camera = m_renderContext->GetCurrentCamera();

    auto mvp = camera->GetProjection() * camera->GetView() * transform;

    m_renderContext->SetDepthTest(material->UseDepthTest);
    material->Use();
    material->SetMat4("MVP", mvp);
    SetActiveTextures(material->GetTextures());

    if(material->RenderMode == material::MeshRenderMode::Triangles){
        mesh->GetGpuBufferObject()->Render(mesh->IndexBuffer.size());
    }
    else
    {
        mesh->GetGpuBufferObject()->RenderLines(mesh->IndexBuffer.size());
    }
}


IRenderContext* GLRenderer::GetRenderContext() const {
    return m_renderContext.get();
}
void GLRenderer::RenderMesh(AnimatedMesh* mesh, material::BaseMaterial* material,
                            const glm::mat4 transform){
    auto camera = m_renderContext->GetCurrentCamera();
    auto mvp = camera->GetProjection() * camera->GetView() * transform;

    auto&anim = mesh->GetAnimationData();

    m_renderContext->SetDepthTest(material->UseDepthTest);

    material->Use();
    material->SetMat4("MVP", mvp);
    material->SetMat4("Bones", anim.current_frame.data(), anim.current_frame.size());
    SetActiveTextures(material->GetTextures());
    mesh->Render();
};

}