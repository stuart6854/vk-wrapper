//
// Created by stumi on 28/02/21.
//

#include "Context.h"
#include "Device.h"

#include <iostream>

auto main() -> int
{
    vkm::Context ctx;
    if (!ctx.Init())
    {
        std::cerr << "Failed to init Context!" << std::endl;
    }

    vkm::Device device(ctx);

    auto cmd = ctx.AllocateCmdBuffer();

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