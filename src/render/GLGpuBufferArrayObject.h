#ifndef GLGPUBUFFERARRAYOBJECT_H
#define GLGPUBUFFERARRAYOBJECT_H

#include "render/IGpuBufferArrayObject.h"
#include "render/IGpuBufferObject.h"
#include "OpenGL.hpp"

namespace render
{
class GLGpuBufferObject;
class GLGpuBufferArrayObject : public IGpuBufferArrayObject
{
public:
    GLGpuBufferArrayObject(const gl::gpu_vertex_array_object_handle& handle);
    virtual ~GLGpuBufferArrayObject();

    virtual void Bind();
    virtual IGpuBufferObject* GetBufferObject(uint32_t index);
    virtual uint32_t GetBufferObjectCount();
    virtual void UpdateBuffer(const BufferDescriptor& desc, uint32_t index);

public:
    bool CreateBufferObjects(BufferDescriptor* descriptors, uint32_t count);

private:
    gl::gpu_vertex_array_object_handle m_handle;
    core::Vector<std::unique_ptr<GLGpuBufferObject>> m_gpuBufferObjects;
};
}

#endif