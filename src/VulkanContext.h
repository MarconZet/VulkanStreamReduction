//
// Created by Marcin on 18.05.2020.
//

#ifndef VULKANSTREAMREDUCE_VULKANCONTEXT_H
#define VULKANSTREAMREDUCE_VULKANCONTEXT_H

#include "vulkan/vulkan.h"


class VulkanContext {
public:
    VulkanContext();

    [[nodiscard]] VkInstance getInstance() const;

    [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const;

    [[nodiscard]] VkDevice getDevice() const;

    [[nodiscard]] uint32_t getComputeFamilyIndex() const;

    [[nodiscard]] VkQueue getQueue() const;

    [[nodiscard]] VkCommandPool getCommandPool() const;

    [[nodiscard]] VkDescriptorPool getDescriptorPool() const;

private:
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    uint32_t computeFamilyIndex;
    VkQueue queue;
    VkCommandPool commandPool;
    VkDescriptorPool descriptorPool;
};


#endif //VULKANSTREAMREDUCE_VULKANCONTEXT_H
