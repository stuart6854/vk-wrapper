//
// Created by stumi on 02/03/21.
//

#include "Swapchain.h"

#include "Helpers.h"

namespace vkm
{
    Swapchain::Swapchain(Context& ctx, Window* window) : m_ctx(ctx), m_window(window)
    {
        m_surface = m_window->CreateSurface(m_ctx.Instance());
        assert(m_ctx.PhysicalDevice().getSurfaceSupportKHR(0, m_surface));

        Recreate(false);
    }

    Swapchain::~Swapchain()
    {
        m_ctx.DeviceWaitIdle();

        m_deletionQueue.Flush();
        m_ctx.Device().destroy(m_swapchain);
        m_ctx.Instance().destroy(m_surface);
    }

    auto Swapchain::ImageCount() const -> uint32_t { return m_images.size(); }

    auto Swapchain::FrameOverlap() const -> uint32_t { return s_frameOverlap; }

    auto Swapchain::CmdBuffer() -> CommandBuffer& { return CurrentFrame().CmdBuffer; }

    void Swapchain::Recreate(bool vsync)
    {
        m_ctx.DeviceWaitIdle();

        auto oldDeletionQueue = m_deletionQueue;
        m_deletionQueue = {};

        auto physicalDevice = m_ctx.PhysicalDevice();

        CreateSwapchain();
        CreateFrames();
        // Recreate swapchain resources
        // Recreate renderpasses
        // Recreate Framebuffers
        // Recreate frame command buffers

        oldDeletionQueue.Flush();
    }

    void Swapchain::BeginFrame()
    {
        auto& frame = CurrentFrame();

        // Wait until the GPU has finished rendering the last frame
        m_ctx.WaitForFence(frame.RenderFence);
        m_ctx.ResetFence(frame.RenderFence);

        // Now that we are sure the commands are finished, we can
        // safely reset the command buffer to begin recording again
        frame.CmdBuffer.Reset();

        // Request image from swapchain
        m_currentImageIndex = m_ctx.Device().acquireNextImageKHR(m_swapchain, UINT64_MAX, frame.PresentComplete, {}).value;

        frame.CmdBuffer.Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    }

    void Swapchain::Present()
    {
        vk::PresentInfoKHR info{};
        info.setSwapchains(m_swapchain);
        info.setImageIndices(m_currentImageIndex);
        info.setWaitSemaphores(CurrentFrame().RenderComplete);

        m_ctx.GraphicsQueue().presentKHR(info);
    }

    void Swapchain::CreateSwapchain()
    {
        const auto& device = m_ctx.Device();

        auto oldSwapchain = m_swapchain;

        auto physicalDevice = m_ctx.PhysicalDevice();
        auto surfaceFormat = ChooseSurfaceFormat(physicalDevice, m_surface);
        auto presentMode = ChoosePresentMode(physicalDevice, m_surface, true);
        auto extent = ChooseExtent(physicalDevice, m_surface, m_window->SurfaceWidth(), m_window->SurfaceHeight());

        auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(m_surface);
        auto imageCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        {
            imageCount = capabilities.maxImageCount;
        }

        vk::SwapchainCreateInfoKHR info{};
        info.setSurface(m_surface);
        info.minImageCount = imageCount;
        info.imageFormat = surfaceFormat.format;
        info.imageColorSpace = surfaceFormat.colorSpace;
        info.imageExtent = extent;
        info.imageArrayLayers = 1;
        info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        info.imageSharingMode = vk::SharingMode::eExclusive;
        info.preTransform = capabilities.currentTransform;
        info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        info.presentMode = presentMode;
        info.clipped = VK_TRUE;
        info.setOldSwapchain(oldSwapchain);

        m_swapchain = device.createSwapchainKHR(info);
        device.destroy(oldSwapchain);

        m_images = device.getSwapchainImagesKHR(m_swapchain);

        m_surfaceFormat = surfaceFormat;
        m_surfaceWidth = extent.width;
        m_surfaceHeight = extent.height;
    }

    void Swapchain::CreateFrames()
    {
        const auto& device = m_ctx.Device();

        vk::FenceCreateInfo fenceInfo{ vk::FenceCreateFlagBits::eSignaled };
        vk::SemaphoreCreateInfo semaphoreInfo{};

        for (int i = 0; i < s_frameOverlap; i++)
        {
            auto& frame = m_frames.at(i);

            frame.RenderFence = device.createFence(fenceInfo);
            frame.PresentComplete = device.createSemaphore(semaphoreInfo);
            frame.RenderComplete = device.createSemaphore(semaphoreInfo);

            frame.CmdBuffer = m_ctx.AllocateCmdBuffer();
            m_deletionQueue.PushDeleter([=]() { m_ctx.DeallocateCmdBuffer(frame.CmdBuffer); });
        }
    }

}  // namespace vkm