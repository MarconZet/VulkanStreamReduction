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
