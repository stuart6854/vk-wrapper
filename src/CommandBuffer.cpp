//
// Created by stumi on 01/03/21.
//

#include "CommandBuffer.h"

namespace vkm
{
    CommandBuffer::CommandBuffer(vk::CommandBuffer cmd) : m_cmd(cmd) {}

    void CommandBuffer::Begin(vk::CommandBufferUsageFlags usage)
    {
        assert(!m_hasBegun);

        vk::CommandBufferBeginInfo info{};
        info.setFlags(usage);

        m_cmd.begin(info);
        m_hasBegun = true;
    }

    void CommandBuffer::BindPipeline(vk::Pipeline pipeline, vk::PipelineBindPoint bindPoint)
    {
        if (m_activePipeline != pipeline)
        {
            m_cmd.bindPipeline(bindPoint, pipeline);
            m_activePipeline = pipeline;
        }
    }

    void CommandBuffer::BindIndexBuffer(vk::Buffer buffer, vk::IndexType indexType)
    {
        if (m_activeIndexBuffer != buffer)
        {
            m_cmd.bindIndexBuffer(buffer, 0, indexType);
            m_activeIndexBuffer = buffer;
        }
    }

    void CommandBuffer::BindVertexBuffer(vk::Buffer buffer)
    {
        if (m_activeVertexBuffer != buffer)
        {
            m_cmd.bindVertexBuffers(0, buffer, {});
            m_activeVertexBuffer = buffer;
        }
    }

    void CommandBuffer::Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
    {
        assert(m_hasBegun);

        m_cmd.dispatch(groupCountX, groupCountY, groupCountZ);
    }

    void CommandBuffer::End()
    {
        assert(m_hasBegun);

        m_cmd.end();
        m_hasBegun = false;
    }

    void CommandBuffer::Submit(vk::Queue queue)
    {
        assert(!m_hasBegun);

        vk::SubmitInfo info{};
        info.setCommandBuffers(m_cmd);

        queue.submit(info);
    }

}  // namespace vkm