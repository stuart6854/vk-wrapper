//
// Created by stumi on 02/03/21.
//

#ifndef VK_WRAPPER_SWAPCHAIN_H
#define VK_WRAPPER_SWAPCHAIN_H

#include "Context.h"
#include "Window.h"
#include "DeletionQueue.h"

#include <vulkan/vulkan.hpp>

namespace vkm
{
    class Swapchain
    {
    public:
        Swapchain(Context& ctx, Window* window);
        ~Swapchain();

        auto ImageCount() const -> uint32_t;
        auto FrameOverlap() const -> uint32_t;

        auto CmdBuffer() -> CommandBuffer&;

        void Recreate(bool vsync);

        void BeginFrame();
        void Present();

    private:
        Context& m_ctx;
        Window* m_window;
        DeletionQueue m_deletionQueue;

        vk::SurfaceKHR m_surface;
        vk::SwapchainKHR m_swapchain;
        uint32_t m_surfaceWidth;
        uint32_t m_surfaceHeight;
        vk::SurfaceFormatKHR m_surfaceFormat;

        std::vector<vk::Image> m_images;
        uint32_t m_currentImageIndex;

        struct Frame
        {
            vk::Semaphore PresentComplete, RenderComplete;
            vk::Fence RenderFence;

            CommandBuffer CmdBuffer;
        };
        static const uint32_t s_frameOverlap = 2;
        std::array<Frame, s_frameOverlap> m_frames;
        uint32_t m_frameCount;

        auto CurrentFrame() -> Frame& { return m_frames.at(m_frameCount % s_frameOverlap); }

        void CreateSwapchain();
        void CreateFrames();
    };
}  // namespace vkm

#endif  // VK_WRAPPER_SWAPCHAIN_H
