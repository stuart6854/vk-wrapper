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

        m_cmdPoolGraphics = CreateCommandPool(m_queueFamilies.graphics.familyIndex);
        m_cmdPoolCompute = CreateCommandPool(m_queueFamilies.graphics.familyIndex);
        m_cmdPoolTransfer = CreateCommandPool(m_queueFamilies.graphics.familyIndex);

        return true;
    }

    auto Context::Init(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device device) -> bool
    {
        m_instance = instance;
        m_physicalDevice = physicalDevice;
        m_device = device;

        return false;
    }

    void Context::DeviceWaitIdle() { m_device.waitIdle(); }

    void Context::WaitForFence(vk::Fence fence) { m_device.waitForFences(fence, VK_TRUE, UINT64_MAX); }

    void Context::ResetFence(vk::Fence fence) { m_device.resetFences(fence); }

    auto Context::AllocateCmdBuffers(int count, vk::CommandBufferLevel level) -> std::vector<CommandBuffer>
    {
        vk::CommandBufferAllocateInfo info{};
        info.setCommandPool(m_cmdPoolGraphics);
        info.setCommandBufferCount(count);
        info.setLevel(level);

        auto allocatedBuffers = m_device.allocateCommandBuffers(info);

        std::vector<CommandBuffer> cmdBuffers;
        cmdBuffers.resize(count);
        for (int i = 0; i < count; i++)
        {
            cmdBuffers[i] = { allocatedBuffers[i] };
        }
        return cmdBuffers;
    }

    auto Context::AllocateCmdBuffer(vk::CommandBufferLevel level) -> CommandBuffer { return AllocateCmdBuffers(1, level)[0]; }

    void Context::DeallocateCmdBuffer(vk::CommandBuffer cmd) { m_device.free(m_cmdPoolGraphics, cmd); }

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

        static const float graphicsQueuePriority = 0.5f;
        static const float computeQueuePriority = 0.5f;
        static const float transferQueuePriority = 1.0f;

        std::vector<float> priorities = { graphicsQueuePriority, computeQueuePriority, transferQueuePriority };

        m_queueFamilies = PickQueueFamilies(m_physicalDevice);
        //        auto queueInfos = GetQueueCreateInfos(m_queueFamilies, &graphicsQueuePriority, &computeQueuePriority, &transferQueuePriority);
        std::vector<vk::DeviceQueueCreateInfo> queueInfos(1);
        queueInfos[0].setQueueFamilyIndex(m_queueFamilies.graphics.familyIndex);
        queueInfos[0].setQueueCount(3);
        queueInfos[0].setQueuePriorities(priorities);

        //        queueInfos[1].setQueueFamilyIndex(m_queueFamilies.compute.familyIndex);
        //        queueInfos[1].setQueueCount(1);
        //        queueInfos[1].pQueuePriorities = &computeQueuePriority;
        //
        //        queueInfos[2].setQueueFamilyIndex(m_queueFamilies.transfer.familyIndex);
        //        queueInfos[2].setQueueCount(1);
        //        queueInfos[2].pQueuePriorities = &transferQueuePriority;

        vk::DeviceCreateInfo createInfo{};
        createInfo.setPEnabledExtensionNames(info.requiredExtensions);
        createInfo.setPEnabledFeatures(&info.enabledFeatures);
        createInfo.setQueueCreateInfos(queueInfos);

        m_device = m_physicalDevice.createDevice(createInfo);
        m_deletionQueue.PushDeleter([=]() { m_device.destroy(); });

        m_queueGraphics = m_device.getQueue(m_queueFamilies.graphics.familyIndex, 0);
        m_queueCompute = m_device.getQueue(m_queueFamilies.graphics.familyIndex, 1);
        m_queueTransfer = m_device.getQueue(m_queueFamilies.graphics.familyIndex, 2);
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

    auto Context::CreateCommandPool(uint32_t familyIndex) -> vk::CommandPool
    {
        vk::CommandPoolCreateInfo info{};
        info.setQueueFamilyIndex(familyIndex);
        info.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

        auto pool = m_device.createCommandPool(info);
        m_deletionQueue.PushDeleter([=]() { m_device.destroy(pool); });

        return pool;
    }

}  // namespace vkm