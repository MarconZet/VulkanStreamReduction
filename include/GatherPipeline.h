//
// Created by Marcin on 22.05.2020.
//

#ifndef VULKANSTREAMREDUCE_GATHERPIPELINE_H
#define VULKANSTREAMREDUCE_GATHERPIPELINE_H


#include "Pipeline.h"

class GatherPipeline : Pipeline {
    VkDescriptorSetLayout createDescriptorSetLayout();
};


#endif //VULKANSTREAMREDUCE_GATHERPIPELINE_H
