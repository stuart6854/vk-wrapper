//
// Created by stumi on 01/03/21.
//

#ifndef VK_WRAPPER_DEVICE_H
#define VK_WRAPPER_DEVICE_H

#include "Context.h"
#include "vk_mem_alloc.h"

namespace vkm
{
    class Device
    {
    public:
        Device(class Context& ctx);
        ~Device();

        auto Context() const -> Context& { return m_ctx; }

    private:
        class Context& m_ctx;
        VmaAllocator m_alloc = nullptr;
    };
}  // namespace vkm

#endif  // VK_WRAPPER_DEVICE_H
