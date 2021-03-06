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

#include <common.h>
#include <MapPipeline.h>

void Filter::init() {
    auto mapShader = shader.getMapShader();

    mapPipeline = MapPipeline(mapShader, device);
    mapPipeline.create();

    //Memory Allocation
    const void* additionalData = shader.getAdditionalData();


    VkPhysicalDeviceMemoryProperties properties;

    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &properties);

    const VkDeviceSize inputBufferSize = shader.getInputElementSize() * 4 * elementNumber;
    const VkDeviceSize outputBufferSize = shader.getOutputElementSize() * 4 * elementNumber;

    const VkDeviceSize stagingBufferSize = std::max(inputBufferSize, outputBufferSize);
    const uint32_t additionalDataSize = std::max(shader.getAdditionalDataSize() * 4,(uint32_t) 256);

    const VkDeviceSize transferMemorySize = stagingBufferSize + additionalDataSize;
    const VkDeviceSize localMemorySize = inputBufferSize + outputBufferSize;

    uint32_t transferMemoryIndex = VK_MAX_MEMORY_TYPES;
    uint32_t localMemoryIndex = VK_MAX_MEMORY_TYPES;

    for (uint32_t k = 0; k < properties.memoryTypeCount; k++) {
        if ((VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT & properties.memoryTypes[k].propertyFlags) &&
            (VK_MEMORY_PROPERTY_HOST_COHERENT_BIT & properties.memoryTypes[k].propertyFlags) &&
            (transferMemorySize < properties.memoryHeaps[properties.memoryTypes[k].heapIndex].size)) {
            transferMemoryIndex = k;
            break;
        }
    }
    for (uint32_t k = 0; k < properties.memoryTypeCount; k++) {
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

    inputBuffer = createBuffer(dst, inputBufferSize, localMemory, 0);
    outputBuffer = createBuffer(src, outputBufferSize, localMemory, inputBufferSize);

    stagingBuffer = createBuffer(src | dst, stagingBufferSize, transferMemory, 0);
    additionalDataBuffer = createBuffer(dst, additionalDataSize, transferMemory, stagingBufferSize);

    void *pointer;
    THROW_ON_FAIL(vkMapMemory(device, transferMemory, stagingBufferSize, additionalDataSize, 0, &pointer))
    memcpy(pointer, additionalData, additionalDataSize);
    vkUnmapMemory(device, transferMemory);

    //Descriptor set allocation

    std::vector<VkBuffer> buffers;
    buffers.push_back(inputBuffer);
    buffers.push_back(outputBuffer);
    buffers.push_back(additionalDataBuffer);
    buffers.push_back(stagingBuffer);

    mapPipeline.createDescriptorSet(buffers, descriptorPool);

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
            0,
            nullptr
    };

    THROW_ON_FAIL(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo))

    const VkBufferCopy bufferCopy = {
            0,
            0,
            inputBufferSize
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
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            0,
            1,
            &memoryBarrier,
            0,
            nullptr,
            0,
            nullptr
    );

    auto set = mapPipeline.getDescriptorSet();
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mapPipeline.getPipeline());
    vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_COMPUTE,
            mapPipeline.getPipelineLayout(),
            0,
            1,
            &set,
            0,
            nullptr
    );
    vkCmdDispatch(commandBuffer, elementNumber / 1024, 1, 1);

    vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            0,
            1,
            &memoryBarrier,
            0,
            nullptr,
            0,
            nullptr
    );

    const VkBufferCopy outputDataCopy = {
            0,
            0,
            outputBufferSize
    };
    vkCmdCopyBuffer(commandBuffer, outputBuffer, stagingBuffer, 1, &outputDataCopy);
    THROW_ON_FAIL(vkEndCommandBuffer(commandBuffer))
}

void * Filter::execute(const void *data) {
    uint32_t inputBufferSize = shader.getInputElementSize() * elementNumber * 4;
    uint32_t outputBufferSize = shader.getOutputElementSize() * elementNumber * 4;

    void *pointer;
    THROW_ON_FAIL(vkMapMemory(device, transferMemory, 0, inputBufferSize, 0, &pointer))
    memcpy(pointer, data, inputBufferSize);
    vkUnmapMemory(device, transferMemory);

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



    THROW_ON_FAIL(vkMapMemory(device, transferMemory, 0, outputBufferSize, 0, (void **) &pointer))
    void * result = calloc(outputBufferSize, 1);
    memcpy(result, pointer, outputBufferSize);
    vkUnmapMemory(device, transferMemory);

    return result;
}

void Filter::unpack(VulkanContext context) {
    physicalDevice = context.getPhysicalDevice();
    device = context.getDevice();
    computeFamilyIndex = context.getComputeFamilyIndex();
    queue = context.getQueue();
    commandPool = context.getCommandPool();
    descriptorPool = context.getDescriptorPool();
}

Filter::Filter(uint32_t elementNumber, Shader shader, VulkanContext vulkanContext)
        : elementNumber(elementNumber), shader(std::move(shader)) {
    if (elementNumber % 16 * 1024 != 0)
        throw std::invalid_argument("Invalid number of elements");
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
