//
// Created by Marcin on 22.05.2020.
//

#ifndef VULKANSTREAMREDUCE_GATHERPIPELINE_H
#define VULKANSTREAMREDUCE_GATHERPIPELINE_H


#include "Pipeline.h"

#include <utility>

class GatherPipeline : public Pipeline {
public:

    void createDescriptorSet(std::vector<VkBuffer> buffers, VkDescriptorPool descriptorPool) override;

    GatherPipeline(std::vector<uint32_t> shader, VkDevice device)
            : Pipeline(std::move(shader), device) {}

protected:
    VkDescriptorSetLayout createDescriptorSetLayout() override;
};


#endif //VULKANSTREAMREDUCE_GATHERPIPELINE_H
