//
// Created by stumi on 28/02/21.
//

#ifndef VK_WRAPPER_DELETIONQUEUE_H
#define VK_WRAPPER_DELETIONQUEUE_H

#include <deque>
#include <functional>

namespace vkm
{
    struct DeletionQueue
    {
        using Deletor = std::function<void()>;

        std::deque<Deletor> deleters;

        void PushDeleter(Deletor&& deleterFunc) { deleters.push_back(deleterFunc); }

        void Flush()
        {
            // Reverse iterate the deletion queue to execute all the functions
            for (auto it = deleters.rbegin(); it != deleters.rend(); it++)
            {
                (*it)();  // Invoke functor
            }
            deleters.clear();
        }
    };
}  // namespace vkm

#endif  // VK_WRAPPER_DELETIONQUEUE_H
