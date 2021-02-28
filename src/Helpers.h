//
// Created by stumi on 28/02/21.
//

#ifndef VK_WRAPPER_HELPERS_H
#define VK_WRAPPER_HELPERS_H

#include <vulkan/vulkan.hpp>

#include <iostream>

namespace vkm
{
    auto CheckLayerSupport(const char* layerName) -> bool
    {
        auto layers = vk::enumerateInstanceLayerProperties();
        for (const auto& layer : layers)
        {
            if (std::strcmp(layer.layerName, layerName) == 0)
            {
                return true;
            }
        }
        return false;
    }

    auto CheckExtensionSupport(const char* extensionName) -> bool
    {
        auto extensions = vk::enumerateInstanceExtensionProperties();
        for (const auto& extension : extensions)
        {
            if (std::strcmp(extension.extensionName, extensionName) == 0)
            {
                return true;
            }
        }
        return false;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData)
    {
        std::cerr << "[Vulkan Validation] " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

}  // namespace vkm

#endif  // VK_WRAPPER_HELPERS_H
