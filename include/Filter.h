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

class Filter {
public:

    Filter(uint32_t elementNumber, Shader shader, VulkanContext vulkanContext);

    void execute();

private:
    uint32_t elementNumber;
    Shader shader;

    VkCommandBuffer commandBuffer;

    VkDeviceMemory memory;
    VkBuffer in_buffer;
    VkBuffer out_buffer;
    VkDescriptorSet descriptorSet;

    Pipeline pipelines[3];

    VkPhysicalDevice physicalDevice;
    VkDevice device;
    uint32_t computeFamilyIndex;
    VkQueue queue;
    VkCommandPool commandPool;
    VkDescriptorPool descriptorPool;

    void unpack(VulkanContext context);

    void init();
};


#endif //VULKANSTREAMREDUCE_FILTER_H
