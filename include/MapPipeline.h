//
// Created by Marcin on 22.05.2020.
//

#ifndef VULKANSTREAMREDUCE_MAPPIPELINE_H
#define VULKANSTREAMREDUCE_MAPPIPELINE_H


#include "Pipeline.h"

class MapPipeline : public Pipeline {
public:
    MapPipeline();

    MapPipeline(std::vector<uint32_t> shader, VkDevice device)
            : Pipeline(std::move(shader), device) {}

    void createDescriptorSet(const std::vector<VkBuffer> &buffers, VkDescriptorPool descriptorPool) override;

protected:
    VkDescriptorSetLayout createDescriptorSetLayout() override;
};


#endif //VULKANSTREAMREDUCE_MAPPIPELINE_H
