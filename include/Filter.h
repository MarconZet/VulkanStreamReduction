//
// Created by Marcin on 18.05.2020.
//

#ifndef VULKANSTREAMREDUCE_FILTER_H
#define VULKANSTREAMREDUCE_FILTER_H

#include <utility>

#include "vulkan/vulkan.h"

#include <VulkanContext.h>
#include <Shader.h>
#include <Pipeline.h>
#include <MapPipeline.h>

class Filter {
public:

    Filter(uint32_t elementNumber, Shader shader, VulkanContext vulkanContext);
    void * execute(const void *data);

private:
    uint32_t elementNumber;
    Shader shader;

    VkCommandBuffer commandBuffer;

    VkBuffer inputBuffer;
    VkBuffer outputBuffer;

    VkBuffer stagingBuffer;
    VkBuffer additionalDataBuffer;

    VkDeviceMemory localMemory;
    VkDeviceMemory transferMemory;

    MapPipeline mapPipeline;

    VkPhysicalDevice physicalDevice;
    VkDevice device;
    uint32_t computeFamilyIndex;
    VkQueue queue;
    VkCommandPool commandPool;
    VkDescriptorPool descriptorPool;

    void unpack(VulkanContext context);
    VkBuffer createBuffer(VkFlags flags, VkDeviceSize size, VkDeviceMemory memory, VkDeviceSize memoryOffset);
    void init();
};


#endif //VULKANSTREAMREDUCE_FILTER_H
