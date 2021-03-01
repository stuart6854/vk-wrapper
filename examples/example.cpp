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

    //    vkm::Device device(ctx);

    return 0;
}