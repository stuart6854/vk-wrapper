//
// Created by stumi on 01/03/21.
//

#include "Device.h"

namespace vkm
{
    Device::Device(class Context& ctx) : m_ctx(ctx)
    {
        VmaAllocatorCreateInfo info{};
        info.instance = ctx.Instance();
        info.physicalDevice = ctx.PhysicalDevice();
        info.device = ctx.Device();

        vmaCreateAllocator(&info, &m_alloc);
    }

    Device::~Device() { vmaDestroyAllocator(m_alloc); }

}  // namespace vkm