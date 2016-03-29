#ifndef OPENGLRENDER_HPP
#define OPENGLRENDER_HPP
#include "GLXW/glxw.h"
#include "render/BufferDescriptor.h"

namespace render
{
namespace gl
{
    struct gpu_shader_handle {
        uint32_t pipeline;
    };

    struct gpu_buffer_object_handle {
        core::generic_data_access_handle data;
        uint32_t bytes;
        uint32_t count;
        uint32_t buffer_id;
        uint32_t type;
        uint32_t data_type;
    };

    struct gpu_vertex_array_object_handle {
        uint32_t vao_id;
    };

    template <class HandleType>
    bool IsHandleValid(const HandleType& handle);
    template <class HandleType>
    void BindHandle(const HandleType& handle);
    template <class HandleType>
    void FreeHandle(const HandleType& handle);
    uint32_t CreateShaderFromString(uint32_t type, const char* source);
    uint32_t CreateProgramPipeline(uint32_t vert_program = 0,
                                   uint32_t frag_program = 0,
                                   uint32_t geom_program = 0);
    gpu_shader_handle CreatePipelineFromShaderStrings(
        const char* vertSource = nullptr, const char* fragSource = nullptr,
        const char* geomSource = nullptr);

    void SetClearColor(const core::pod::Vec3<int32_t>& color);

    /// Implementation

    inline bool IsHandleValid(const gpu_shader_handle& handle)
    {
        return handle.pipeline != 0;
    }

    inline bool IsHandleValid(const gpu_buffer_object_handle& handle)
    {
        return handle.buffer_id != 0;
    }

    inline bool IsHandleValid(const gpu_vertex_array_object_handle& handle)
    {
        return handle.vao_id != 0;
    }

    inline void BindHandle(const gpu_shader_handle& handle)
    {
        glBindProgramPipeline(handle.pipeline);
    }

    inline void BindHandle(const gpu_buffer_object_handle& handle)
    {
        glBindBuffer(handle.type, handle.buffer_id);
    }

    inline void BindHandle(const gpu_vertex_array_object_handle& handle)
    {
        glBindVertexArray(handle.vao_id);
    }

    inline void FreeHandle(const gpu_shader_handle& handle)
    {
        glDeleteProgramPipelines(1, &handle.pipeline);
    }

    inline void FreeHandle(const gpu_buffer_object_handle& handle)
    {
        glDeleteBuffers(1, &handle.buffer_id);
    }

    inline void FreeHandle(const gpu_vertex_array_object_handle& handle)
    {
        glDeleteVertexArrays(1, &handle.vao_id);
    }

    inline uint32_t CreateShaderFromString(uint32_t type, const char* source)
    {
        if (source && source[0] != core::string::NullTerminator)
            return glCreateShaderProgramv(type, 1, (const char**)&source);
        else
            return 0;
    }

    inline uint32_t CreateProgramPipeline(uint32_t vert_program,
                                          uint32_t frag_program,
                                          uint32_t geom_program)
    {
        uint32_t ppo;
        glCreateProgramPipelines(1, &ppo);

        if (vert_program)
            glUseProgramStages(ppo, GL_VERTEX_SHADER_BIT, vert_program);
        if (frag_program)
            glUseProgramStages(ppo, GL_FRAGMENT_SHADER_BIT, frag_program);
        if (geom_program)
            glUseProgramStages(ppo, GL_GEOMETRY_SHADER_BIT, geom_program);

        return ppo;
    }

    inline gpu_shader_handle CreatePipelineFromShaderStrings(
        const char* vertSource, const char* fragSource, const char* geomSource)
    {
        return gpu_shader_handle{
            .pipeline = CreateProgramPipeline(
                CreateShaderFromString(GL_VERTEX_SHADER, vertSource),
                CreateShaderFromString(GL_FRAGMENT_SHADER, fragSource),
                CreateShaderFromString(GL_GEOMETRY_SHADER, geomSource))};
    }

    inline void CreateGpuStorage(gpu_buffer_object_handle* handles,
                                 uint32_t count)
    {
        auto buffers = core::UniquePtr<uint32_t[]>(new uint32_t[count]);
        glGenBuffers(count, buffers.get());

        for (uint32_t i = 0; i < count; i++) {
            handles[i].buffer_id = buffers.get()[i];
        }
    }

    inline void UpdateBufferObject(const gpu_buffer_object_handle& handle)
    {
        glBufferData(handle.type, handle.count * handle.bytes,
                     (void*)handle.data.ptr, GL_STATIC_DRAW);
    }

    inline gpu_vertex_array_object_handle CreateVertexArrayObject()
    {
        gpu_vertex_array_object_handle handle;
        glGenVertexArrays(1, &handle.vao_id);
        return handle;
    }

    inline void SetClearColor(const core::pod::Vec3<int32_t>& color)
    {
        glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, 1);
    }

    inline void Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    inline uint32_t GetGLBufferObjectType(render::BufferObjectType type)
    {
        return type == render::BufferObjectType::index ? GL_ELEMENT_ARRAY_BUFFER
                                                       : GL_ARRAY_BUFFER;
    }
}
}

#endif