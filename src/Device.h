//
// Created by stumi on 01/03/21.
//

#ifndef VK_WRAPPER_DEVICE_H
#define VK_WRAPPER_DEVICE_H

#include "Context.h"
#include "DeletionQueue.h"
#include "Resources.h"
#include "vk_mem_alloc.h"

#include <unordered_map>

namespace vkm
{
    class Device
    {
    public:
        Device(class Context& ctx);
        ~Device();

        auto Context() const -> Context& { return m_ctx; }

        auto MapBuffer(vk::Buffer buffer) -> void*;
        void UnmapBuffer(vk::Buffer buffer);

        auto CreateBuffer(uint32_t size, vk::BufferUsageFlagBits usage, VmaMemoryUsage memUsage, const void* initialData = nullptr) -> vk::Buffer;

        void SetBufferData(vk::Buffer buffer, uint32_t size, const void* data);

    private:
        class Context& m_ctx;
        DeletionQueue m_deletionQueue;
        VmaAllocator m_allocator = nullptr;

        std::unordered_map<vk::Buffer, Buffer> m_bufferMap;

        auto MapMemory(VmaAllocation alloc) -> void*;
        void UnmapMemory(VmaAllocation alloc);
    };
}  // namespace vkm

#endif  // VK_WRAPPER_DEVICE_H
