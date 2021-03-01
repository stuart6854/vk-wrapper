//
// Created by stumi on 28/02/21.
//

#ifndef VK_WRAPPER_CONTEXT_H
#define VK_WRAPPER_CONTEXT_H

#include "DeletionQueue.h"
#include "Helpers.h"

#include <vulkan/vulkan.hpp>

namespace vkm
{
    enum APIVersion : uint32_t
    {
        API_1_0 = VK_API_VERSION_1_0,
        API_1_1 = VK_API_VERSION_1_1,
        API_1_2 = VK_API_VERSION_1_2,
    };

    struct Version
    {
        uint32_t major;
        uint32_t minor;
        uint32_t patch;

        operator uint32_t() const { return VK_MAKE_VERSION(major, minor, patch); }
    };

    struct AppInfo
    {
        std::string appName;
        Version appVersion;
        std::string engineName;
        Version engineVersion;
        APIVersion apiVersion;
    };

    struct InstanceInfo
    {
        AppInfo appInfo{};
        std::vector<const char*> requiredExtensions{};
    };

    struct DeviceInfo
    {
        std::vector<const char*> requiredExtensions{};
        const vk::PhysicalDeviceFeatures& enabledFeatures{};
    };

    class Context
    {
    public:
        Context() = default;
        ~Context();

        auto Init(const InstanceInfo& instanceInfo = {}, const DeviceInfo& deviceInfo = {}, bool useValidation = true) -> bool;
        auto Init(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device device) -> bool;

        auto DeletionQueue() -> DeletionQueue& { return m_deletionQueue; }

        auto Instance() const -> vk::Instance { return m_instance; }
        auto PhysicalDevice() const -> vk::PhysicalDevice { return m_physicalDevice; }
        auto Device() const -> vk::Device { return m_device; }

    private:
        struct DeletionQueue m_deletionQueue;

        vk::Instance m_instance;
        vk::PhysicalDevice m_physicalDevice;
        vk::Device m_device;

        vk::DebugUtilsMessengerEXT m_debugMessenger;

        QueueFamilies m_queueFamilies;
        vk::Queue m_graphicsQueue;
        vk::Queue m_transferQueue;

        void CreateInstance(const InstanceInfo& info, bool useValidation);
        void PickPhysicalDevice();
        void CreateDevice(const DeviceInfo& info);

        void CreateDebugMessenger();
    };

}  // namespace vkm

#endif  // VK_WRAPPER_CONTEXT_H
