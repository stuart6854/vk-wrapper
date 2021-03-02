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

    struct Queue
    {
        vk::QueueFlagBits flag;
        uint32_t familyIndex = VK_QUEUE_FAMILY_IGNORED;
        uint32_t queueIndex = 0;

        auto IsValid() const { return familyIndex != VK_QUEUE_FAMILY_IGNORED; }
        auto IsGraphics() const -> bool { return flag == vk::QueueFlagBits::eGraphics; };
        auto IsCompute() const -> bool { return flag == vk::QueueFlagBits::eCompute; };
        auto IsTransfer() const -> bool { return flag == vk::QueueFlagBits::eTransfer; };
    };

    struct QueueFamilies
    {
        Queue graphics;
        Queue compute;
        Queue transfer;

        std::vector<vk::DeviceCreateInfo> queueInfo;
    };

    auto PickQueueFamilies(vk::PhysicalDevice physicalDevice) -> QueueFamilies;

    auto GetQueueCreateInfos(const QueueFamilies& families) -> std::vector<vk::DeviceQueueCreateInfo>;

    VKAPI_ATTR auto VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                             void* pUserData) -> VkBool32;

    auto ChooseSurfaceFormat(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) -> vk::SurfaceFormatKHR;

    auto ChoosePresentMode(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, uint8_t vsync) -> vk::PresentModeKHR;

    auto ChooseExtent(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, uint32_t width, uint32_t height) -> vk::Extent2D;

}  // namespace vkm

#endif  // VK_WRAPPER_HELPERS_H
