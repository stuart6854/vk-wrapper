//
// Created by stumi on 01/03/21.
//

#include "Device.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace vkm
{
    Device::Device(class Context& ctx) : m_ctx(ctx)
    {
        VmaAllocatorCreateInfo info{};
        info.instance = ctx.Instance();
        info.physicalDevice = ctx.PhysicalDevice();
        info.device = ctx.Device();

        vmaCreateAllocator(&info, &m_allocator);

        m_deletionQueue.PushDeleter([=]() { vmaDestroyAllocator(m_allocator); });
    }

    Device::~Device() { m_deletionQueue.Flush(); }

    auto Device::MapBuffer(vk::Buffer buffer) -> void*
    {
        const auto& bufferRes = m_bufferMap[buffer];
        return MapMemory(bufferRes.alloc);
    }

    void Device::UnmapBuffer(vk::Buffer buffer)
    {
        const auto& bufferRes = m_bufferMap[buffer];
        UnmapMemory(bufferRes.alloc);
    }

    auto Device::CreateBuffer(uint32_t size, vk::BufferUsageFlagBits usage, VmaMemoryUsage memUsage, const void* initialData) -> vk::Buffer
    {
        vk::BufferCreateInfo info{};
        info.setSize(size);
        info.setUsage(usage);
        info.setSharingMode(vk::SharingMode::eExclusive);
        VkBufferCreateInfo bufferInfo = info;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = memUsage;

        VkBuffer buffer{};
        VmaAllocation alloc{};
        vmaCreateBuffer(m_allocator, &bufferInfo, &allocInfo, &buffer, &alloc, nullptr);

        m_bufferMap[buffer] = { size, buffer, alloc };

        m_deletionQueue.PushDeleter([=]() { vmaDestroyBuffer(m_allocator, buffer, alloc); });

        if (initialData != nullptr)
        {
            SetBufferData(buffer, size, initialData);
        }

        return buffer;
    }

    void Device::SetBufferData(vk::Buffer buffer, uint32_t size, const void* data)
    {
        const auto& bufferRes = m_bufferMap[buffer];

        // Make sure we can't copy more than the buffers size
        auto copySize = std::min(size, bufferRes.size);

        auto* mappedData = MapMemory(bufferRes.alloc);
        std::memcpy(mappedData, data, copySize);
        UnmapMemory(bufferRes.alloc);
    }

    auto Device::MapMemory(VmaAllocation alloc) -> void*
    {
        void* mapped = nullptr;
        vmaMapMemory(m_allocator, alloc, &mapped);
        return mapped;
    }

    void Device::UnmapMemory(VmaAllocation alloc) { vmaUnmapMemory(m_allocator, alloc); }

}  // namespace vkm