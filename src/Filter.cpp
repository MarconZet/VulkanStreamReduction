//
// Created by Marcin on 18.05.2020.
//
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "vulkan/vulkan.h"

#include "Filter.h"

#include "common.h"
#include <ScatterPipeline.h>
#include <GatherPipeline.h>
#include <PrefixPipeline.h>

void Filter::init() {
    auto prefixShader = shader.getShader("prefix.spv");
    auto gatherShader = shader.getShader("gather.spv");
    auto scatterShader = shader.getScatterShader();

    scatterPipeline = ScatterPipeline(scatterShader, device);
    prefixPipeline = PrefixPipeline(gatherShader, device);
    gatherPipeline = GatherPipeline(gatherShader, device);




    //Memory Allocation
    const std::vector<uint8_t> additionalData = shader.getAdditionalData();

    VkPhysicalDeviceMemoryProperties properties;

    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &properties);

    const VkDeviceSize dataBufferSize = shader.getElementSize() * elementNumber;
    const VkDeviceSize prefixBufferSize = elementNumber * 4;
    const VkDeviceSize algorithmDataBufferSize = 3 * 4;

    const VkDeviceSize transferMemorySize = dataBufferSize + algorithmDataBufferSize + additionalData.size();
    const VkDeviceSize localMemorySize = 2 * dataBufferSize + prefixBufferSize;

    uint32_t transferMemoryIndex = VK_MAX_MEMORY_TYPES;
    uint32_t localMemoryIndex = VK_MAX_MEMORY_TYPES;

    for (uint32_t k = 0; k < properties.memoryTypeCount; k++) {
        if ((VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT & properties.memoryTypes[k].propertyFlags) &&
            (VK_MEMORY_PROPERTY_HOST_COHERENT_BIT & properties.memoryTypes[k].propertyFlags) &&
            (transferMemorySize < properties.memoryHeaps[properties.memoryTypes[k].heapIndex].size)) {
            transferMemoryIndex = k;
            break;
        }
        if ((VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT & properties.memoryTypes[k].propertyFlags) &&
            (localMemorySize < properties.memoryHeaps[properties.memoryTypes[k].heapIndex].size)) {
            localMemoryIndex = k;
            break;
        }
    }

    if (transferMemoryIndex == VK_MAX_MEMORY_TYPES || localMemoryIndex == VK_MAX_MEMORY_TYPES) {
        throw std::runtime_error("Failed to find suitable memory types\n");
    }

    const VkMemoryAllocateInfo memoryAllocateInfo[2] = {
            {
                    VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                    nullptr,
                    transferMemorySize,
                    transferMemoryIndex
            },
            {
                    VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                    nullptr,
                    localMemorySize,
                    localMemoryIndex
            }
    };

    THROW_ON_FAIL(vkAllocateMemory(device, &memoryAllocateInfo[0], nullptr, &transferMemory))
    THROW_ON_FAIL(vkAllocateMemory(device, &memoryAllocateInfo[1], nullptr, &localMemory))

    const auto dst = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    const auto src = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    inputBuffer = createBuffer(dst, dataBufferSize, localMemory, 0);
    outputBuffer = createBuffer(src, dataBufferSize, localMemory, dataBufferSize);
    prefixBuffer = createBuffer(0, prefixBufferSize, localMemory, 2 * dataBufferSize);

    stagingBuffer = createBuffer(src | dst, dataBufferSize, transferMemory, 0);
    additionalDataBuffer = createBuffer(dst, additionalData.size(), transferMemory, dataBufferSize);
    algorithmDataBuffer = createBuffer(src | dst, algorithmDataBufferSize, transferMemory,
                                       dataBufferSize + additionalData.size());

    //Descriptor set allocation

    std::vector<VkBuffer> buffers(6);
    buffers.push_back(inputBuffer);
    buffers.push_back(outputBuffer);
    buffers.push_back(prefixBuffer);
    buffers.push_back(stagingBuffer);
    buffers.push_back(additionalDataBuffer);
    buffers.push_back(algorithmDataBuffer);

    scatterPipeline.createDescriptorSet(buffers, descriptorPool);
    gatherPipeline.createDescriptorSet(buffers, descriptorPool);
    prefixPipeline.createDescriptorSet(buffers, descriptorPool);


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

    const VkBufferCopy bufferCopy = {
            0,
            0,
            VK_WHOLE_SIZE
    };
    vkCmdCopyBuffer(commandBuffer, stagingBuffer, inputBuffer, 1, &bufferCopy);

    const VkMemoryBarrier memoryBarrier = {
            VK_STRUCTURE_TYPE_MEMORY_BARRIER,
            nullptr,
            VK_ACCESS_MEMORY_WRITE_BIT,
            VK_ACCESS_MEMORY_READ_BIT
    };

    vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            0,
            1,
            &memoryBarrier,
            0,
            nullptr,
            0,
            nullptr
    );

    auto set = scatterPipeline.getDescriptorSet();
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, scatterPipeline.getPipeline());
    vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_COMPUTE,
            scatterPipeline.getPipelineLayout(),
            0,
            1,
            &set,
            0,
            nullptr
    );
    vkCmdDispatch(commandBuffer, elementNumber / 1024, 1, 1);

    set = prefixPipeline.getDescriptorSet();
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, prefixPipeline.getPipeline());
    vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_COMPUTE,
            prefixPipeline.getPipelineLayout(),
            0,
            1,
            &set,
            0,
            nullptr
    );
    vkCmdDispatch(commandBuffer, elementNumber / 16 * 1024, 1, 1);

    set = gatherPipeline.getDescriptorSet();
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, gatherPipeline.getPipeline());
    vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_COMPUTE,
            gatherPipeline.getPipelineLayout(),
            0,
            1,
            &set,
            0,
            nullptr
    );
    vkCmdDispatch(commandBuffer, elementNumber / 1024, 1, 1);

    THROW_ON_FAIL(vkEndCommandBuffer(commandBuffer))
}

void Filter::execute() {

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

VkBuffer
Filter::createBuffer(VkFlags flags, VkDeviceSize size, VkDeviceMemory memory, VkDeviceSize memoryOffset) {
    VkBuffer buffer;

    const VkBufferCreateInfo buffersCreateInfo = {
            VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            nullptr,
            0,
            size,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | flags,
            VK_SHARING_MODE_EXCLUSIVE,
            1,
            &computeFamilyIndex
    };

    THROW_ON_FAIL(vkCreateBuffer(device, &buffersCreateInfo, nullptr, &buffer))

    THROW_ON_FAIL(vkBindBufferMemory(device, buffer, memory, memoryOffset))

    return buffer;
}
