#pragma once

#include "rne_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace rne {

    class RneDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(RneDevice& rneDevice) : rneDevice{ rneDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<RneDescriptorSetLayout> build() const;

        private:
            RneDevice& rneDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        RneDescriptorSetLayout(
            RneDevice& rneDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~RneDescriptorSetLayout();
        RneDescriptorSetLayout(const RneDescriptorSetLayout&) = delete;
        RneDescriptorSetLayout& operator=(const RneDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        RneDevice& rneDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class RneDescriptorWriter;
    };

    class RneDescriptorPool {
    public:
        class Builder {
        public:
            Builder(RneDevice& rneDevice) : rneDevice{ rneDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<RneDescriptorPool> build() const;

        private:
            RneDevice& rneDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        RneDescriptorPool(
            RneDevice& rneDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~RneDescriptorPool();
        RneDescriptorPool(const RneDescriptorPool&) = delete;
        RneDescriptorPool& operator=(const RneDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        RneDevice& rneDevice;
        VkDescriptorPool descriptorPool;

        friend class RneDescriptorWriter;
    };

    class RneDescriptorWriter {
    public:
        RneDescriptorWriter(RneDescriptorSetLayout& setLayout, RneDescriptorPool& pool);

        RneDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        RneDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        RneDescriptorSetLayout& setLayout;
        RneDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace rne