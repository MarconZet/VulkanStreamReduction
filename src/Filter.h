//
// Created by Marcin on 18.05.2020.
//

#ifndef VULKANSTREAMREDUCE_FILTER_H
#define VULKANSTREAMREDUCE_FILTER_H

#include "vulkan/vulkan.h"

#include "VulkanContext.h"

class Filter {
public:

    Filter(uint32_t elementSize, uint32_t elementNumber, VulkanContext vulkanContext) : elementSize(elementSize), elementNumber(elementNumber) {
        unpack(vulkanContext);
        init();
    }

    void execute();

private:
    uint32_t elementSize;
    uint32_t elementNumber;

    VkCommandBuffer commandBuffer;

    VkDeviceMemory memory;
    VkBuffer in_buffer;
    VkBuffer out_buffer;
    VkDescriptorSet descriptorSet;

    VkShaderModule shader_module;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;

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
