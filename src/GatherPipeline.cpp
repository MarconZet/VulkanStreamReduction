//
// Created by Marcin on 22.05.2020.
//

#include <common.h>
#include "GatherPipeline.h"

VkDescriptorSetLayout GatherPipeline::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[4] = {
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
            },
            {
                    2,
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    1,
                    VK_SHADER_STAGE_COMPUTE_BIT,
                    nullptr
            },
            {
                    2,
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
            4,
            descriptorSetLayoutBindings
    };

    THROW_ON_FAIL(vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout))
    return descriptorSetLayout;
}

void GatherPipeline::createDescriptorSet(std::vector<VkBuffer> buffers, VkDescriptorPool descriptorPool) {
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            nullptr,
            descriptorPool,
            1,
            &descriptorSetLayout
    };


    THROW_ON_FAIL(vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet))

    VkDescriptorBufferInfo descriptorBufferInfos[4] = {
            {
                    buffers[0],
                    0,
                    VK_WHOLE_SIZE
            },
            {
                    buffers[1],
                    0,
                    VK_WHOLE_SIZE
            },
            {
                    buffers[2],
                    0,
                    VK_WHOLE_SIZE
            },
            {
                    buffers[5],
                    0,
                    VK_WHOLE_SIZE
            }
    };

    VkWriteDescriptorSet writeDescriptorSet[4] = {
            {
                    VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    nullptr,
                    descriptorSet,
                    0,
                    0,
                    1,
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    nullptr,
                    &descriptorBufferInfos[0],
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
                    &descriptorBufferInfos[1],
                    nullptr
            },
            {
                    VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    nullptr,
                    descriptorSet,
                    2,
                    0,
                    1,
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    nullptr,
                    &descriptorBufferInfos[2],
                    nullptr
            },
            {
                    VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    nullptr,
                    descriptorSet,
                    3,
                    0,
                    1,
                    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    nullptr,
                    &descriptorBufferInfos[3],
                    nullptr
            }
    };

    vkUpdateDescriptorSets(device, 4, writeDescriptorSet, 0, nullptr);


}
