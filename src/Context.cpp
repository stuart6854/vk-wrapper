//
// Created by stumi on 28/02/21.
//

#include "Context.h"

namespace vkm
{
    Context::~Context()
    {
        // Make sure the GPU has stopped doing things
        //        vkDeviceWaitIdle(m_device);
        m_deletionQueue.Flush();
    }

    auto Context::Init(const InstanceInfo& instanceInfo, const DeviceInfo& deviceInfo, bool useValidation) -> bool
    {
        CreateInstance(instanceInfo, useValidation);
        PickPhysicalDevice();
        CreateDevice(deviceInfo);

        return true;
    }

    auto Context::Init(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device device) -> bool
    {
        m_instance = instance;
        m_physicalDevice = physicalDevice;
        m_device = device;

        return false;
    }

    void Context::CreateInstance(const InstanceInfo& info, bool useValidation)
    {
        vk::ApplicationInfo appInfo{};
        appInfo.setPApplicationName(info.appInfo.appName.c_str());
        appInfo.setPEngineName(info.appInfo.engineName.c_str());
        appInfo.setApplicationVersion(info.appInfo.appVersion);
        appInfo.setEngineVersion(info.appInfo.engineVersion);
        appInfo.setApiVersion(info.appInfo.apiVersion);

        std::vector<const char*> layers;
        std::vector<const char*> extensions = info.requiredExtensions;
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
        createInfo.setPApplicationInfo(&appInfo);
        createInfo.setPEnabledLayerNames(layers);
        createInfo.setPEnabledExtensionNames(extensions);

        m_instance = vk::createInstance(createInfo);
        m_deletionQueue.PushDeleter([=]() { m_instance.destroy(); });

        if (useValidation)
        {
            CreateDebugMessenger();
        }
    }

    void Context::PickPhysicalDevice()
    {
        auto possibleGPUs = m_instance.enumeratePhysicalDevices();

        // TODO: Score all possible GPUs and pick one with best score

        // Right now we just pick any Discrete gpu, if available
        for (const auto& gpu : possibleGPUs)
        {
            auto properties = gpu.getProperties();
            if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            {
                m_physicalDevice = gpu;
            }
        }

        // If there are no Discrete GPUs, just pick the first GPU
        if (!m_physicalDevice)
        {
            m_physicalDevice = possibleGPUs[0];
        }
    }

    void Context::CreateDevice(const DeviceInfo& info)
    {
        auto queueProperties = m_physicalDevice.getQueueFamilyProperties();

        m_queueFamilies = PickQueueFamilies(m_physicalDevice);
        std::vector<vk::DeviceQueueCreateInfo> queueInfos(2);
        queueInfos[0].setQueueFamilyIndex(m_queueFamilies.graphics.familyIndex);
        queueInfos[0].setQueueCount(1);
        queueInfos[0].pQueuePriorities = &graphicsQueuePriority;

        queueInfos[1].setQueueFamilyIndex(m_queueFamilies.transfer.familyIndex);
        queueInfos[1].setQueueCount(1);
        queueInfos[1].pQueuePriorities = &transferQueuePriority;

        vk::DeviceCreateInfo createInfo{};
        createInfo.setPEnabledExtensionNames(info.requiredExtensions);
        createInfo.setPEnabledFeatures(&info.enabledFeatures);
        createInfo.setQueueCreateInfos(queueInfos);

        m_device = m_physicalDevice.createDevice(createInfo);
        m_deletionQueue.PushDeleter([=]() { m_device.destroy(); });

        m_graphicsQueue = m_device.getQueue(m_queueFamilies.graphics.familyIndex, 0);
        m_transferQueue = m_device.getQueue(m_queueFamilies.transfer.familyIndex, 0);
    }

    void Context::CreateDebugMessenger()
    {
        vk::DebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.setMessageSeverity(/*vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |*/ vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
        createInfo.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                                  vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);
        createInfo.setPfnUserCallback(debugCallback);

        auto dispatcher = vk::DispatchLoaderDynamic(m_instance, vkGetInstanceProcAddr);

        m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(createInfo, nullptr, dispatcher);
        m_deletionQueue.PushDeleter([=]() { m_instance.destroy(m_debugMessenger, nullptr, dispatcher); });
    }

}  // namespace vkm