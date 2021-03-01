//
// Created by stumi on 28/02/21.
//

#ifndef VK_WRAPPER_HELPERS_H
#define VK_WRAPPER_HELPERS_H

#include <vulkan/vulkan.hpp>

#include <iostream>

namespace vkm
{
    static const float graphicsQueuePriority = 0.5f;
    static const float computeQueuePriority = 0.5f;
    static const float transferQueuePriority = 1.0f;

    auto CheckLayerSupport(const char* layerName) -> bool;

    auto CheckExtensionSupport(const char* extensionName) -> bool;

    struct QueueFamily
    {
        uint32_t familyIndex = VK_QUEUE_FAMILY_IGNORED;
        uint32_t queueCount;
        vk::QueueFlags flags;

        auto IsValid() const { return familyIndex != VK_QUEUE_FAMILY_IGNORED; }
        auto IsGraphics() const -> bool { return (flags & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics; };
        auto IsCompute() const -> bool { return (flags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute; };
        auto IsTransfer() const -> bool { return (flags & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits::eTransfer; };
    };

    struct QueueFamilies
    {
        QueueFamily graphics;
        QueueFamily compute;
        QueueFamily transfer;
    };

    auto PickQueueFamilies(vk::PhysicalDevice physicalDevice) -> QueueFamilies;

    auto GetQueueCreateInfos(const QueueFamilies& families) -> std::vector<vk::DeviceQueueCreateInfo>;

    VKAPI_ATTR auto VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void* pUserData) -> VkBool32;

}  // namespace vkm

#endif  // VK_WRAPPER_HELPERS_H
