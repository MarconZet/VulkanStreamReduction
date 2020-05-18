//
// Created by Marcin on 18.05.2020.
//
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

#include "vulkan/vulkan.h"

#include "common.h"
#include "VulkanContext.h"


VkResult vkGetBestComputeQueueNPH(VkPhysicalDevice physicalDevice, uint32_t *queueFamilyIndex);

VulkanContext::VulkanContext() {
    const VkApplicationInfo applicationInfo = {
            VK_STRUCTURE_TYPE_APPLICATION_INFO,
            nullptr,
            "StreamReduce",
            0,
            "",
            0,
            VK_MAKE_VERSION(1, 1, 0)
    };

    const VkInstanceCreateInfo instanceCreateInfo = {
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            nullptr,
            0,
            &applicationInfo,
            0,
            nullptr,
            0,
            nullptr
    };

    THROW_ON_FAIL(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

    uint32_t physicalDeviceCount = 0;
    THROW_ON_FAIL(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr));

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);

    THROW_ON_FAIL(vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()));
    physicalDevice = physicalDevices[0];

    computeFamilyIndex = 0;
    THROW_ON_FAIL(vkGetBestComputeQueueNPH(physicalDevice, &computeFamilyIndex));

    const float queuePriority = 1.0f;
    const VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            nullptr,
            0,
            computeFamilyIndex,
            1,
            &queuePriority
    };

    const VkDeviceCreateInfo deviceCreateInfo = {
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            nullptr,
            0,
            1,
            &deviceQueueCreateInfo,
            0,
            nullptr,
            0,
            nullptr,
            nullptr
    };

    THROW_ON_FAIL(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device));



    VkDescriptorPoolSize descriptorPoolSize = {
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            100
    };

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
            VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            nullptr,
            0,
            20,
            1,
            &descriptorPoolSize
    };

    THROW_ON_FAIL(vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool));

    VkCommandPoolCreateInfo commandPoolCreateInfo = {
            VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            nullptr,
            0,
            computeFamilyIndex
    };

    THROW_ON_FAIL(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool));

    vkGetDeviceQueue(device, computeFamilyIndex, 0, &queue);
}

VkResult vkGetBestComputeQueueNPH(VkPhysicalDevice physicalDevice, uint32_t *queueFamilyIndex) {
    uint32_t queueFamilyPropertiesCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertiesCount);

    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.data());

    for (uint32_t i = 0; i < queueFamilyPropertiesCount; i++) {
        const VkQueueFlags maskedFlags = (~(VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT) &
                                          queueFamilyProperties[i].queueFlags);

        if (!(VK_QUEUE_GRAPHICS_BIT & maskedFlags) && (VK_QUEUE_COMPUTE_BIT & maskedFlags)) {
            *queueFamilyIndex = i;
            return VK_SUCCESS;
        }
    }

    for (uint32_t i = 0; i < queueFamilyPropertiesCount; i++) {
        const VkQueueFlags maskedFlags = (~(VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT) &
                                          queueFamilyProperties[i].queueFlags);

        if (VK_QUEUE_COMPUTE_BIT & maskedFlags) {
            *queueFamilyIndex = i;
            return VK_SUCCESS;
        }
    }

    return VK_ERROR_INITIALIZATION_FAILED;
}

VkInstance VulkanContext::getInstance() const {
    return instance;
}

VkPhysicalDevice VulkanContext::getPhysicalDevice() const {
    return physicalDevice;
}

VkDevice VulkanContext::getDevice() const {
    return device;
}

uint32_t VulkanContext::getComputeFamilyIndex() const {
    return computeFamilyIndex;
}

VkQueue VulkanContext::getQueue() const {
    return queue;
}

VkCommandPool VulkanContext::getCommandPool() const {
    return commandPool;
}

VkDescriptorPool VulkanContext::getDescriptorPool() const {
    return descriptorPool;
}
