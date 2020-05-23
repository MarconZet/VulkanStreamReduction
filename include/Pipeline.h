//
// Created by Marcin on 21.05.2020.
//

#ifndef VULKANSTREAMREDUCE_PIPELINE_H
#define VULKANSTREAMREDUCE_PIPELINE_H

#include <vector>
#include <stdexcept>

#include "vulkan/vulkan.h"

class Pipeline {
public:
    Pipeline(std::vector<uint32_t> shader , VkDevice device) : shader(std::move(shader)) , device(device) {}

    void create();

    [[nodiscard]] VkPipeline getPipeline() const;

protected:
    VkShaderModule shaderModule;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    VkDevice device;

    virtual VkDescriptorSetLayout createDescriptorSetLayout() = 0;

private:
    std::vector<uint32_t> shader;

    VkShaderModule createShaderModule();

    VkPipelineLayout createPipelineLayout();

    VkPipeline createPipeline();
};


#endif //VULKANSTREAMREDUCE_PIPELINE_H
