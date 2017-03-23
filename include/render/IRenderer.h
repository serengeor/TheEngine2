#ifndef IRENDERER_H
#define IRENDERER_H

#include "CFrameBufferObject.h"
#include "CRenderBufferObject.h"

namespace render {
// Note: remove these
using Vec2i = core::pod::Vec2<int32_t>;
using Vec2f = core::pod::Vec2<float>;
using Vec3i = core::pod::Vec3<int32_t>;
using Vec3f = core::pod::Vec3<float>;
using Vec4i = core::pod::Vec4<int32_t>;
using Vec4f = core::pod::Vec4<float>;

class IGpuProgram;
class IGpuBufferObject;
class IGpuBufferArrayObject;
class IRendererDebugMessageMonitor;
class ITexture;
class IFrameBufferObject;
class IRenderBufferObject;

struct BufferDescriptor;
struct TextureDescriptor;
struct FrameBufferObjectDescriptor;
class ITexture;

class IRenderer
{
public:
    virtual ~IRenderer()
    {
    }
    virtual IRendererDebugMessageMonitor* GetDebugMessageMonitor() = 0;
    virtual core::SharedPtr<IGpuProgram> CreateProgram(const core::String& vertSource = "",
                                                       const core::String& fragSource = "",
                                                       const core::String& geomSource = "") = 0;
    virtual core::SharedPtr<IGpuBufferArrayObject> CreateBufferArrayObject(
        const core::Vector<BufferDescriptor>& descriptors)                               = 0;
    virtual core::SharedPtr<ITexture> CreateTexture(const TextureDescriptor& descriptor) = 0;
    virtual core::SharedPtr<IFrameBufferObject> CreateFrameBufferObject(
        const FrameBufferObjectDescriptor& descriptor) = 0;
    virtual core::SharedPtr<IRenderBufferObject> CreateRenderBufferObject(
        const RenderBufferObjectDescriptor& descriptor)                                     = 0;
    virtual void SetActiveTextures(const core::Vector<core::SharedPtr<ITexture>>& textures) = 0;
    virtual void SetActiveFrameBuffer(core::SharedPtr<IFrameBufferObject> fbo,
                                      FrameBufferTarget target) = 0;
    virtual void SetClearColor(const Vec3i& color)              = 0;
    virtual void Clear()                                        = 0;
};
}

#endif
