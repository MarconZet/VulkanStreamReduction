//
// Created by Marcin on 18.05.2020.
//
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "vulkan/vulkan.h"

#include "common.h"
#include "Filter.h"

void Filter::init() {
    auto prefixShader = shader.getShader("prefix.spv"),
    auto gatherShader = shader.getShader("gather.spv"),

    const std::vector<uint32_t> *shaders[4] = {
            &shader.getScatterShader(),
            &prefixShader,
            &gatherShader,
            &shader.getCopyShader()
    };


    for (int i = 0; i < 4; i++) {

    }

    VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[2] = {
            {
                    0,
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    1,
                    VK_SHADER_STAGE_COMPUTE_BIT,
                    nullptr
            },
            {
                    1,
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    1,
                    VK_SHADER_STAGE_COMPUTE_BIT,
                    nullptr
            }
    };

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            nullptr,
            0,
            2,
            descriptorSetLayoutBindings
    };

    THROW_ON_FAIL(vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout))

    //Memory Allocation

    VkPhysicalDeviceMemoryProperties properties;

    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &properties);

    const int32_t bufferLength = elementNumber;

    const uint32_t bufferSize = elementSize * bufferLength;

    const VkDeviceSize memorySize = bufferSize * 2;

    uint32_t memoryTypeIndex = VK_MAX_MEMORY_TYPES;

    for (uint32_t k = 0; k < properties.memoryTypeCount; k++) {
        if ((VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT & properties.memoryTypes[k].propertyFlags) &&
            (VK_MEMORY_PROPERTY_HOST_COHERENT_BIT & properties.memoryTypes[k].propertyFlags) &&
            (memorySize < properties.memoryHeaps[properties.memoryTypes[k].heapIndex].size)) {
            memoryTypeIndex = k;
            break;
        }
    }

    THROW_ON_FAIL(memoryTypeIndex == VK_MAX_MEMORY_TYPES ? VK_ERROR_OUT_OF_HOST_MEMORY : VK_SUCCESS)

    const VkMemoryAllocateInfo memoryAllocateInfo = {
            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            nullptr,
            memorySize,
            memoryTypeIndex
    };

    THROW_ON_FAIL(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &memory))


    const VkBufferCreateInfo bufferCreateInfo = {
            VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            nullptr,
            0,
            bufferSize,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_SHARING_MODE_EXCLUSIVE,
            1,
            &computeFamilyIndex
    };

    THROW_ON_FAIL(vkCreateBuffer(device, &bufferCreateInfo, nullptr, &in_buffer))

    THROW_ON_FAIL(vkBindBufferMemory(device, in_buffer, memory, 0))

    THROW_ON_FAIL(vkCreateBuffer(device, &bufferCreateInfo, nullptr, &out_buffer))

    THROW_ON_FAIL(vkBindBufferMemory(device, out_buffer, memory, bufferSize))

    //Descriptor set allocation and updating

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            nullptr,
            descriptorPool,
            1,
            &descriptorSetLayout
    };


    THROW_ON_FAIL(vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet))

    VkDescriptorBufferInfo in_descriptorBufferInfo = {
            in_buffer,
            0,
            VK_WHOLE_SIZE
    };

    VkDescriptorBufferInfo out_descriptorBufferInfo = {
            out_buffer,
            0,
            VK_WHOLE_SIZE
    };

    VkWriteDescriptorSet writeDescriptorSet[2] = {
            {
                    VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    nullptr,
                    descriptorSet,
                    0,
                    0,
                    1,
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    nullptr,
                    &in_descriptorBufferInfo,
                    nullptr
            },
            {
                    VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    nullptr,
                    descriptorSet,
                    1,
                    0,
                    1,
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    nullptr,
                    &out_descriptorBufferInfo,
                    nullptr
            }
    };

    vkUpdateDescriptorSets(device, 2, writeDescriptorSet, 0, nullptr);


    //Command buffer recording


    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            nullptr,
            commandPool,
            VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            1
    };

    THROW_ON_FAIL(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer))

    VkCommandBufferBeginInfo commandBufferBeginInfo = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            nullptr,
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            nullptr
    };

    THROW_ON_FAIL(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo))

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                            pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

    vkCmdDispatch(commandBuffer, bufferSize / sizeof(int32_t), 1, 1);

    THROW_ON_FAIL(vkEndCommandBuffer(commandBuffer))
}

void Filter::execute() {
    const int32_t bufferLength = elementNumber;

    const uint32_t bufferSize = elementSize * bufferLength;

    const VkDeviceSize memorySize = bufferSize * 2;

    int32_t *payload;
    THROW_ON_FAIL(vkMapMemory(device, memory, 0, memorySize, 0, (void **) &payload))

    for (uint32_t k = 1; k < memorySize / elementSize; k++) {
        payload[k] = rand();
    }

    vkUnmapMemory(device, memory);


    VkSubmitInfo submitInfo = {
            VK_STRUCTURE_TYPE_SUBMIT_INFO,
            nullptr,
            0,
            nullptr,
            nullptr,
            1,
            &commandBuffer,
            0,
            nullptr
    };

    THROW_ON_FAIL(vkQueueSubmit(queue, 1, &submitInfo, nullptr))

    THROW_ON_FAIL(vkQueueWaitIdle(queue))

    THROW_ON_FAIL(vkMapMemory(device, memory, 0, memorySize, 0, (void **) &payload))

    for (uint32_t k = 0, e = bufferSize / sizeof(int32_t); k < e; k++) {
        THROW_ON_FAIL(payload[k + e] == payload[k] ? VK_SUCCESS : VK_ERROR_OUT_OF_HOST_MEMORY)
    }
}

void Filter::unpack(VulkanContext context) {
    physicalDevice = context.getPhysicalDevice();
    device = context.getDevice();
    computeFamilyIndex = context.getComputeFamilyIndex();
    queue = context.getQueue();
    commandPool = context.getCommandPool();
    descriptorPool = context.getDescriptorPool();
}

Filter::Filter(uint32_t elementNumber, Shader shader, VulkanContext vulkanContext) : elementNumber(elementNumber),
                                                                                     shader(std::move(shader)) {
    unpack(vulkanContext);
    init();
}
