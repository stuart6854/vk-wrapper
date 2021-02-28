//
// Created by stumi on 28/02/21.
//

#ifndef VK_WRAPPER_CONTEXT_H
#define VK_WRAPPER_CONTEXT_H

#include "DeletionQueue.h"

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

    class Context
    {
    public:
        Context(AppInfo appInfo = {}, bool useValidation = true, std::vector<const char*> requiredExts = {}, std::vector<const char*> requiredDeviceExts = {});
        Context(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device device);
        ~Context();

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

        void CreateInstance(const AppInfo& appInfo, bool useValidation, std::vector<const char*> requiredInstanceExts);
        void PickPhysicalDevice();
        void CreateDevice(std::vector<const char*> requiredDeviceExts);

        void CreateDebugMessenger();
    };

}  // namespace vkm

#endif  // VK_WRAPPER_CONTEXT_H
