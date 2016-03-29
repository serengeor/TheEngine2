#ifndef GLGPUBUFFEROBJECT_H
#define GLGPUBUFFEROBJECT_H

#include "render/IGpuBufferObject.h"
#include "OpenGL.hpp"

namespace render
{
class GLGpuBufferObject : public IGpuBufferObject
{
public:
    GLGpuBufferObject(const gl::gpu_buffer_object_handle& handle);
    virtual ~GLGpuBufferObject();

    virtual void Bind();
    virtual void UpdateBuffer(const BufferDescriptor& desc);

private:
    virtual void UpdateHandle(const BufferDescriptor& desc);

private:
    gl::gpu_buffer_object_handle m_handle;
};
}

#endif