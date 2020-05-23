//
// Created by Marcin on 22.05.2020.
//

#ifndef VULKANSTREAMREDUCE_SCATTERPIPELINE_H
#define VULKANSTREAMREDUCE_SCATTERPIPELINE_H


#include "Pipeline.h"

class ScatterPipeline : public Pipeline {
public:
    ScatterPipeline(std::vector<uint32_t> shader, VkDevice device)
            : Pipeline(std::move(shader), device) {}

    void createDescriptorSet(std::vector<VkBuffer> buffers, VkDescriptorPool descriptorPool) override;

protected:
    VkDescriptorSetLayout createDescriptorSetLayout() override;
};


#endif //VULKANSTREAMREDUCE_SCATTERPIPELINE_H
