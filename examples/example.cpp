//
// Created by stumi on 28/02/21.
//

#include "Context.h"

#include <iostream>

auto main() -> int
{
    vkm::Context ctx;
    if (!ctx.Init())
    {
        std::cerr << "Failed to init Context!" << std::endl;
    }

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

    //    vkm::Device device(ctx);

    return 0;
}