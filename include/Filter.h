//
// Created by Marcin on 18.05.2020.
//

#ifndef VULKANSTREAMREDUCE_FILTER_H
#define VULKANSTREAMREDUCE_FILTER_H

#include <utility>

#include "vulkan/vulkan.h"

#include "VulkanContext.h"
#include "Shader.h"
#include "Pipeline.h"
#include <ScatterPipeline.h>
#include <GatherPipeline.h>
#include <PrefixPipeline.h>

class Filter {
public:

    Filter(uint32_t elementNumber, Shader shader, VulkanContext vulkanContext);

    void execute(const std::vector<uint8_t>& data);

private:
    uint32_t elementNumber;
    Shader shader;

    VkCommandBuffer commandBuffer;

    VkBuffer inputBuffer;
    VkBuffer outputBuffer;
    VkBuffer prefixBuffer;

    VkBuffer stagingBuffer;
    VkBuffer additionalDataBuffer;
    VkBuffer algorithmDataBuffer;

    VkDeviceMemory localMemory;
    VkDeviceMemory transferMemory;

    ScatterPipeline scatterPipeline;
    GatherPipeline gatherPipeline;
    PrefixPipeline prefixPipeline;

    VkPhysicalDevice physicalDevice;
    VkDevice device;
    uint32_t computeFamilyIndex;
    VkQueue queue;
    VkCommandPool commandPool;
    VkDescriptorPool descriptorPool;

    void unpack(VulkanContext context);
    VkBuffer createBuffer(VkFlags flags, VkDeviceSize size, VkDeviceMemory memory, VkDeviceSize memoryOffset);
    void copyCommandBuffer(VkBuffer dst, VkBuffer src, VkDeviceSize size);
    void init();
};


#endif //VULKANSTREAMREDUCE_FILTER_H
