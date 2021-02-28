//
// Created by stumi on 28/02/21.
//

#include "Context.h"

#include <utility>

#include "Helpers.h"

namespace vkm
{
    Context::Context(AppInfo appInfo, bool useValidation, std::vector<const char*> requiredExts, std::vector<const char*> requiredDeviceExts)
    {
        CreateInstance(appInfo, useValidation, std::move(requiredExts));
        PickPhysicalDevice();
        CreateDevice(std::move(requiredDeviceExts));
    }

    Context::Context(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device device)
        : m_instance(instance), m_physicalDevice(physicalDevice), m_device(device)
    {
    }

    Context::~Context()
    {
        // Make sure the GPU has stopped doing things
        //        vkDeviceWaitIdle(m_device);
        m_deletionQueue.Flush();
    }

    void Context::CreateInstance(const AppInfo& appInfo, bool useValidation, std::vector<const char*> requiredInstanceExts)
    {
        vk::ApplicationInfo info{};
        info.setPApplicationName(appInfo.appName.c_str());
        info.setPEngineName(appInfo.engineName.c_str());
        info.setApplicationVersion(appInfo.appVersion);
        info.setEngineVersion(appInfo.engineVersion);
        info.setApiVersion(appInfo.apiVersion);

        std::vector<const char*> layers;
        std::vector<const char*> extensions = std::move(requiredInstanceExts);
        if (useValidation)
        {
            const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
            if (CheckLayerSupport(validationLayerName))
            {
                layers.push_back(validationLayerName);
            }
            if (CheckExtensionSupport(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
            {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }
        }

        vk::InstanceCreateInfo createInfo{};
        createInfo.setPApplicationInfo(&info);
        createInfo.setPEnabledLayerNames(layers);
        createInfo.setPEnabledExtensionNames(extensions);

        m_instance = vk::createInstance(createInfo);
        m_deletionQueue.PushDeleter([=]() { m_instance.destroy(); });

        if (useValidation)
        {
            CreateDebugMessenger();
        }
    }

    void Context::PickPhysicalDevice() {}

    void Context::CreateDevice(std::vector<const char*> requiredDeviceExts) {}

    void Context::CreateDebugMessenger()
    {
        vk::DebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
        createInfo.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                                  vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);
        createInfo.setPfnUserCallback(debugCallback);

        auto dispatcher = vk::DispatchLoaderDynamic(m_instance, vkGetInstanceProcAddr);

        m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(createInfo, nullptr, dispatcher);
        m_deletionQueue.PushDeleter([=]() { m_instance.destroy(m_debugMessenger, nullptr, dispatcher); });
    }

}  // namespace vkm