//
// Created by stumi on 02/03/21.
//

#ifndef VK_WRAPPER_WINDOW_H
#define VK_WRAPPER_WINDOW_H

#include <vulkan/vulkan.hpp>

namespace vkm
{
    class Window
    {
    public:
        virtual auto SurfaceWidth() const -> int = 0;
        virtual auto SurfaceHeight() const -> int = 0;

        virtual auto InstanceExtensions() -> std::vector<const char*> = 0;
        virtual auto DeviceExtensions() -> std::vector<const char*> = 0;

        virtual auto CreateSurface(vk::Instance instance) const -> vk::SurfaceKHR = 0;
    };

}  // namespace vkm

#endif  // VK_WRAPPER_WINDOW_H
