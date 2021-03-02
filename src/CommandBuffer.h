//
// Created by stumi on 01/03/21.
//

#ifndef VK_WRAPPER_COMMANDBUFFER_H
#define VK_WRAPPER_COMMANDBUFFER_H

#include <vulkan/vulkan.hpp>

namespace vkm
{
    class CommandBuffer
    {
    public:
        CommandBuffer() = default;
        CommandBuffer(vk::CommandBuffer cmd);

        void Begin(vk::CommandBufferUsageFlags usage);

        void BindPipeline(vk::Pipeline pipeline, vk::PipelineBindPoint bindPoint);

        void BindIndexBuffer(vk::Buffer buffer, vk::IndexType indexType);
        void BindVertexBuffer(vk::Buffer buffer);

        void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

        void End();

        void Submit(vk::Queue queue);

        void Reset();

        operator vk::CommandBuffer() const { return m_cmd; }

    private:
        vk::CommandBuffer m_cmd;
        bool m_hasBegun = false;

        vk::Pipeline m_activePipeline = nullptr;

        vk::Buffer m_activeIndexBuffer = nullptr;
        vk::Buffer m_activeVertexBuffer = nullptr;
    };
}  // namespace vkm

#endif  // VK_WRAPPER_COMMANDBUFFER_H
