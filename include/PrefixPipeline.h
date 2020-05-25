//
// Created by Marcin on 22.05.2020.
//

#ifndef VULKANSTREAMREDUCE_PREFIXPIPELINE_H
#define VULKANSTREAMREDUCE_PREFIXPIPELINE_H


#include "Pipeline.h"

class PrefixPipeline : public Pipeline {
public:
    PrefixPipeline();

    PrefixPipeline(std::vector<uint32_t> shader , VkDevice device)
    : Pipeline(std::move(shader), device){}

    void createDescriptorSet(std::vector<VkBuffer> buffers, VkDescriptorPool descriptorPool) override;

protected:
    VkDescriptorSetLayout createDescriptorSetLayout() override;
};


#endif //VULKANSTREAMREDUCE_PREFIXPIPELINE_H
