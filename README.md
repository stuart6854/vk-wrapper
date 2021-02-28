# vk-wrapper

## Required

- Instance
- Physical Device
- Device
- Command Buffers
- Swapchain
    - Recreation (screen resize)
- Renderpass
- Pipelines
    - Pipeline Layout
    - Graphics
    - Compute
- Buffers
- Images

## Design

- Context
    - Owns a VkInstance, VkPhysicalDevice and VkDevice.
    - Deletion queue
    - Submit(Queue, CommandBuffer)
- Device
    - Holds reference to a Context instance.
    - Used to create/destroy resources
    - Keeps track of all created resources/objects
- Window (abstract)
    - Instance/Device extensions
    - Get Surface
- Swapchain
    - Recreate
    - Present
- CommandBuffer