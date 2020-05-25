//
// Created by Marcin on 21.05.2020.
//

#include <utility>
#include <exception>
#include <cstdlib>

#include "vulkan/vulkan.h"

#include "common.h"
#include "Pipeline.h"

void Pipeline::create(){
    createShaderModule();
    createDescriptorSetLayout();
    createPipelineLayout();
    createPipeline();
}

VkShaderModule Pipeline::createShaderModule() {
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {
            VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            nullptr,
            0,
            shader.size() * 4,
            shader.data()
    };
    THROW_ON_FAIL(vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule))
    return shaderModule;
}

VkPipelineLayout Pipeline::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            nullptr,
            0,
            1,
            &descriptorSetLayout,
            0,
            nullptr
    };

    THROW_ON_FAIL(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout))
    return pipelineLayout;
}

VkPipeline Pipeline::createPipeline() {
    VkComputePipelineCreateInfo computePipelineCreateInfo = {
            VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
            nullptr,
            0,
            {
                    VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    nullptr,
                    0,
                    VK_SHADER_STAGE_COMPUTE_BIT,
                    shaderModule,
                    "main",
                    nullptr
            },
            pipelineLayout,
            nullptr,
            0
    };

    THROW_ON_FAIL(vkCreateComputePipelines(device, nullptr, 1, &computePipelineCreateInfo, nullptr, &pipeline))
    return pipeline;
}

VkPipeline Pipeline::getPipeline() const {
    return pipeline;
}

Pipeline::Pipeline() {}


VkDescriptorSet Pipeline::getDescriptorSet() const {
    return descriptorSet;
}

VkPipelineLayout Pipeline::getPipelineLayout() const {
    return pipelineLayout;
}
