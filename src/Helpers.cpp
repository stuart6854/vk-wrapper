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
            auto queueProps = queueProperties[0];

            Queue queue{};
            queue.familyIndex = 0;

            if ((queueProps.queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics)
            {
                queue.flag = vk::QueueFlagBits::eGraphics;
                queue.familyIndex++;
                families.graphics = queue;
            }
            if ((queueProps.queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute)
            {
                queue.flag = vk::QueueFlagBits::eCompute;
                queue.familyIndex++;
                families.graphics = queue;
            }
            if ((queueProps.queueFlags & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits::eTransfer)
            {
                queue.flag = vk::QueueFlagBits::eTransfer;
                queue.familyIndex++;
                families.graphics = queue;
            }

            return families;
        }

        // Find dedicated transfer queue family
        for (uint32_t i = 0; i < queueProperties.size(); i++)
        {
            auto& family = queueProperties[i];
            if ((family.queueFlags & vk::QueueFlagBits::eTransfer) && !(family.queueFlags & vk::QueueFlagBits::eGraphics) &&
                !(family.queueFlags & vk::QueueFlagBits::eCompute))
            {
                families.transfer = { vk::QueueFlagBits::eTransfer, i, family.queueCount };
                break;
            }
        }

        // Find dedicated graphics queue family
        for (uint32_t i = 0; i < queueProperties.size(); i++)
        {
            auto& family = queueProperties[i];
            if ((family.queueFlags & vk::QueueFlagBits::eGraphics) && !(family.queueFlags & vk::QueueFlagBits::eCompute))
            {
                families.graphics = { vk::QueueFlagBits::eGraphics, i, family.queueCount };
                break;
            }
        }

        // Find dedicated compute queue family
        for (uint32_t i = 0; i < queueProperties.size(); i++)
        {
            auto& family = queueProperties[i];
            if ((family.queueFlags & vk::QueueFlagBits::eCompute) && !(family.queueFlags & vk::QueueFlagBits::eGraphics))
            {
                families.compute = { vk::QueueFlagBits::eCompute, i, family.queueCount };
                break;
            }
        }

        // If dedicated queues were not found, find the first queue available
        for (uint32_t i = 0; i < queueProperties.size(); i++)
        {
            auto& family = queueProperties[i];
            if (!families.transfer.IsValid())
            {
                if (family.queueFlags & vk::QueueFlagBits::eTransfer)
                {
                    families.transfer = { vk::QueueFlagBits::eTransfer, i, family.queueCount };
                }
            }
            if (!families.graphics.IsValid())
            {
                if (family.queueFlags & vk::QueueFlagBits::eGraphics)
                {
                    families.graphics = { vk::QueueFlagBits::eGraphics, i, family.queueCount };
                }
            }
            if (!families.compute.IsValid())
            {
                if (family.queueFlags & vk::QueueFlagBits::eCompute)
                {
                    families.compute = { vk::QueueFlagBits::eCompute, i, family.queueCount };
                }
            }
        }

        if (families.compute.familyIndex == families.graphics.familyIndex)
        {
            families.compute.queueIndex = families.graphics.queueIndex + 1;
        }
        if (families.transfer.familyIndex == families.graphics.familyIndex)
        {
            families.transfer.queueIndex = families.graphics.queueIndex + 1;
        }
        else if (families.transfer.familyIndex == families.compute.familyIndex)
        {
            families.transfer.queueIndex = families.compute.queueIndex + 1;
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

    auto ChooseSurfaceFormat(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) -> vk::SurfaceFormatKHR
    {
        auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);

        for (const auto& format : surfaceFormats)
        {
            if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            {
                return format;
            }
        }
        return surfaceFormats[0];
    }

    auto ChoosePresentMode(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, uint8_t vsync) -> vk::PresentModeKHR
    {
        if (vsync > 2)
        {
            vsync = 2;
        }

        if (vsync == 2)
        {
            // Double buffering
            return vk::PresentModeKHR::eFifo;
        }
        if (vsync == 1)
        {
            // Triple buffering
            return vk::PresentModeKHR::eMailbox;
        }

        // No vsync
        return vk::PresentModeKHR::eImmediate;
    }

    auto ChooseExtent(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, uint32_t width, uint32_t height) -> vk::Extent2D
    {
        auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }

        vk::Extent2D actualExtent = { width, height };

        // Use Min/Max to clamp the values between the allowed minimum and
        // maximum extents that are supported
        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }

}  // namespace vkm