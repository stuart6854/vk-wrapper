//
// Created by stumi on 01/03/21.
//

#ifndef VK_WRAPPER_RESOURCES_H
#define VK_WRAPPER_RESOURCES_H

#include "vk_mem_alloc.h"

#include <vulkan/vulkan.hpp>

namespace vkm
{
    struct Buffer
    {
        uint32_t size = 0;
        vk::Buffer buffer = nullptr;
        VmaAllocation alloc = nullptr;
    };
}  // namespace vkm

#endif  // VK_WRAPPER_RESOURCES_H
