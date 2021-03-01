//
// Created by stumi on 01/03/21.
//

#include "Helpers.h"

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

    auto PickQueueFamilies(vk::PhysicalDevice physicalDevice) -> QueueFamilies
    {
        auto queueProperties = physicalDevice.getQueueFamilyProperties();

        QueueFamilies families{};

        // Handle single queue family case
        if (queueProperties.size() == 1)
        {
            families.graphics = families.transfer = families.transfer = { 0, queueProperties[0].queueCount, queueProperties[0].queueFlags };
            return families;
        }

        // Find dedicated transfer queue family
        for (uint32_t i = 0; i < queueProperties.size(); i++)
        {
            auto& family = queueProperties[i];
            if ((family.queueFlags & vk::QueueFlagBits::eTransfer) && !(family.queueFlags & vk::QueueFlagBits::eGraphics) &&
                !(family.queueFlags & vk::QueueFlagBits::eCompute))
            {
                families.transfer = { i, family.queueCount, family.queueFlags };
                break;
            }
        }

        // Find dedicated graphics queue family
        for (uint32_t i = 0; i < queueProperties.size(); i++)
        {
            auto& family = queueProperties[i];
            if ((family.queueFlags & vk::QueueFlagBits::eGraphics) && !(family.queueFlags & vk::QueueFlagBits::eCompute))
            {
                families.transfer = { i, family.queueCount, family.queueFlags };
                break;
            }
        }

        // Find dedicated compute queue family
        for (uint32_t i = 0; i < queueProperties.size(); i++)
        {
            auto& family = queueProperties[i];
            if ((family.queueFlags & vk::QueueFlagBits::eCompute) && !(family.queueFlags & vk::QueueFlagBits::eGraphics))
            {
                families.transfer = { i, family.queueCount, family.queueFlags };
                break;
            }
        }

        // Find first queue family that supports the requested flags,
        // for other queue types or if no dedicated queue types were found
        for (uint32_t i = 0; i < queueProperties.size(); i++)
        {
            auto& family = queueProperties[i];
            if (!families.transfer.IsValid())
            {
                if (family.queueFlags & vk::QueueFlagBits::eTransfer)
                {
                    families.transfer = { i, family.queueCount, family.queueFlags };
                }
            }
            if (!families.graphics.IsValid())
            {
                if (family.queueFlags & vk::QueueFlagBits::eGraphics)
                {
                    families.graphics = { i, family.queueCount, family.queueFlags };
                }
            }
            if (!families.compute.IsValid())
            {
                if (family.queueFlags & vk::QueueFlagBits::eCompute)
                {
                    families.compute = { i, family.queueCount, family.queueFlags };
                }
            }
        }
        return families;
    }

    auto GetQueueCreateInfos(const QueueFamilies& families) -> std::vector<vk::DeviceQueueCreateInfo>
    {
        // TODO: DeviceQueueCreateInfo doesn't retain priorities

        std::vector<vk::DeviceQueueCreateInfo> queueInfos;

        uint32_t graphicsCount = 0;
        uint32_t computeCount = 0;
        uint32_t transferCount = 0;

        std::vector<float> graphicsPriorities;
        std::vector<float> computePriorities;
        std::vector<float> transferPriorities;

        graphicsCount++;
        graphicsPriorities.push_back(graphicsQueuePriority);

        if (families.compute.familyIndex != families.graphics.familyIndex)
        {
            computeCount++;
            computePriorities.push_back(computeQueuePriority);
        }
        else
        {
            graphicsCount++;
            graphicsPriorities.push_back(computeQueuePriority);
        }

        if (families.transfer.familyIndex != families.graphics.familyIndex && families.transfer.familyIndex != families.compute.familyIndex)
        {
            transferCount++;
            transferPriorities.push_back(transferQueuePriority);
        }
        else if (families.transfer.familyIndex == families.graphics.familyIndex)
        {
            graphicsCount++;
            graphicsPriorities.push_back(transferQueuePriority);
        }
        else if (families.transfer.familyIndex == families.compute.familyIndex)
        {
            computeCount++;
            computePriorities.push_back(transferQueuePriority);
        }

        if (graphicsCount > 0)
        {
            vk::DeviceQueueCreateInfo queue{};
            queue.setQueueFamilyIndex(families.graphics.familyIndex);
            queue.setQueueCount(graphicsCount);
            queue.setQueuePriorities(graphicsPriorities);

            queueInfos.push_back(queue);
        }
        if (computeCount > 0)
        {
            vk::DeviceQueueCreateInfo queue{};
            queue.setQueueFamilyIndex(families.compute.familyIndex);
            queue.setQueueCount(computeCount);
            queue.setQueuePriorities(computePriorities);

            queueInfos.push_back(queue);
        }
        if (transferCount > 0)
        {
            vk::DeviceQueueCreateInfo queue{};
            queue.setQueueFamilyIndex(families.transfer.familyIndex);
            queue.setQueueCount(transferCount);
            queue.setQueuePriorities(transferPriorities);

            queueInfos.push_back(queue);
        }

        return queueInfos;
    }

    auto debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                       void* pUserData) -> VkBool32
    {
        std::cerr << "[Vulkan Validation] " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
}  // namespace vkm