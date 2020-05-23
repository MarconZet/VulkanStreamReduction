//
// Created by Marcin on 22.05.2020.
//
#include <vector>


#include <common.h>
#include "ScatterPipeline.h"

VkDescriptorSetLayout ScatterPipeline::createDescriptorSetLayout() {
     VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[3] = {
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
            }

    };

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            nullptr,
            0,
            3,
            descriptorSetLayoutBindings
    };

    THROW_ON_FAIL(vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout))
    return descriptorSetLayout;
}
