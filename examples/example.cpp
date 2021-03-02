//
// Created by stumi on 28/02/21.
//

#include "Context.h"
#include "Device.h"
#include "Swapchain.h"

#include <GLFW/glfw3.h>

#include <iostream>

class Window : public vkm::Window
{
public:
    Window()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_handle = glfwCreateWindow(m_width, m_height, "VKM - VulkanWrapper", nullptr, nullptr);
    }

    ~Window()
    {
        glfwDestroyWindow(m_handle);
        glfwTerminate();
    }

    void PollEvents() const { glfwPollEvents(); }

    auto ShouldClose() const -> bool { return glfwWindowShouldClose(m_handle); };

    auto SurfaceWidth() const -> int override { return m_width; }

    auto SurfaceHeight() const -> int override { return m_height; }

    auto InstanceExtensions() -> std::vector<const char*> override
    {
        uint32_t count{};
        const char** extensions = glfwGetRequiredInstanceExtensions(&count);

        return std::vector<const char*>(extensions, extensions + count);
    }

    auto DeviceExtensions() -> std::vector<const char*> override { return { VK_KHR_SWAPCHAIN_EXTENSION_NAME }; }

    auto CreateSurface(vk::Instance instance) const -> vk::SurfaceKHR override
    {
        VkSurfaceKHR surface = nullptr;
        glfwCreateWindowSurface(instance, m_handle, nullptr, &surface);
        return surface;
    }

private:
    GLFWwindow* m_handle;
    uint32_t m_width = 720;
    uint32_t m_height = 600;
};

auto main() -> int
{
    Window window;

    vkm::InstanceInfo instanceInfo{};
    instanceInfo.requiredExtensions = window.InstanceExtensions();

    vkm::DeviceInfo deviceInfo{};
    deviceInfo.requiredExtensions = window.DeviceExtensions();

    vkm::Context ctx;
    if (!ctx.Init(instanceInfo, deviceInfo))
    {
        std::cerr << "Failed to init Context!" << std::endl;
    }

    vkm::Device device(ctx);

    float data[4] = { 1.0f, 2.0f, 4.0f, 8.0f };
    auto buffer = device.CreateBuffer(4 * sizeof(float), vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_ONLY, data);

    vkm::Swapchain swapchain(ctx, &window);

    swapchain.Recreate(false);

    bool running = true;
    while (running)
    {
        window.PollEvents();

        swapchain.BeginFrame();

        auto& cmd = swapchain.CmdBuffer();

        swapchain.Present();

        if (window.ShouldClose())
        {
            running = false;
        }
    }

    //    auto cmd = ctx.AllocateCmdBuffer();
    //
    //    cmd.Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    //
    //    cmd.BindPipeline({}, vk::PipelineBindPoint::eCompute);
    //
    //    cmd.Dispatch(1, 1, 1);
    //
    //    cmd.End();
    //
    //    cmd.Submit(ctx.ComputeQueue());
    //
    //    ctx.ComputeQueue().waitIdle();

    return 0;
}