//
// Created by Marcin on 22.05.2020.
//

#ifndef VULKANSTREAMREDUCE_PREFIXPIPELINE_H
#define VULKANSTREAMREDUCE_PREFIXPIPELINE_H


#include "Pipeline.h"

class PrefixPipeline : Pipeline {
    VkDescriptorSetLayout createDescriptorSetLayout();
};


#endif //VULKANSTREAMREDUCE_PREFIXPIPELINE_H
