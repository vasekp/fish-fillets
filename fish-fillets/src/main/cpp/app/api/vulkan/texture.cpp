#include "api/vulkan.h"

namespace vulkan {

struct Texture::Impl {
    Display& display;
    vk::raii::Image image;
    vk::raii::DeviceMemory memory;
    vk::raii::ImageView imageView;
    TextureType type;
    const vk::DescriptorSet* descriptorSet;
};

Texture::Texture(Display& display, USize size, TextureType type, std::uint8_t* data) :
    pImpl{},
    m_size(size)
{
    auto format = type.channels() == 4 ? vk::Format::eR8G8B8A8Unorm : vk::Format::eR8Unorm;
    vk::raii::Image image{display.device(), vk::ImageCreateInfo{}
        .setImageType(vk::ImageType::e2D)
        .setExtent({m_size.width, m_size.height, 1})
        .setMipLevels(1)
        .setArrayLayers(1)
        .setFormat(format)
        .setTiling(vk::ImageTiling::eOptimal)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setUsage(data
                ? vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst
                : vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive)
        .setSamples(vk::SampleCountFlagBits::e1)};

    vk::raii::DeviceMemory deviceMemory{display.allocMemory(image, vk::MemoryPropertyFlagBits::eDeviceLocal)};

    vk::raii::ImageView imageView{display.device(), vk::ImageViewCreateInfo{}
            .setImage(*image)
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(format)
            .setSubresourceRange(baseRange)};

    pImpl = std::make_unique<Impl>(Impl{
        display,
        std::move(image),
        std::move(deviceMemory),
        std::move(imageView),
        type,
        display.descriptors().allocDescriptorSet(type.binding())});

    if(data) {
        replaceData(data);
    } else {
        auto& commandBuffer = display.commandBuffer();
        auto barrierU2S = vk::ImageMemoryBarrier{}
            .setImage(*pImpl->image)
            .setSubresourceRange(baseRange)
            .setOldLayout(vk::ImageLayout::eUndefined)
            .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setSrcAccessMask(vk::AccessFlagBits::eNone)
            .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        commandBuffer.reset({});
        commandBuffer.begin(vk::CommandBufferBeginInfo{}.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eFragmentShader,
                    {}, {}, {}, {barrierU2S});
        commandBuffer.end();

        display.queue().submit(vk::SubmitInfo{}.setCommandBuffers(commandBuffer), nullptr);
        display.queue().waitIdle();
    }

    const auto& sampler = type == TextureType::mask ? display.samplerNearest() : display.samplerLinear();
    auto imageInfo = vk::DescriptorImageInfo{}
        .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImageView(*pImpl->imageView)
        .setSampler(sampler);
    const auto& descriptorSet = *pImpl->descriptorSet;
    auto dWrite = vk::WriteDescriptorSet{}
        .setDstSet(descriptorSet)
        .setDstBinding(0)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setDescriptorCount(1)
        .setPImageInfo(&imageInfo);
    display.device().updateDescriptorSets({dWrite}, {});
}

Texture::Texture(Texture&& other) noexcept = default;

Texture& Texture::operator=(Texture&& other) noexcept {
    m_size = other.m_size;
    std::swap(pImpl, other.pImpl); // so that the old one is properly freed
    return *this;
}

Texture::~Texture() {
    if(pImpl)
        pImpl->display.descriptors().freeDescriptorSet(pImpl->type.binding(), pImpl->descriptorSet);
}

const vk::Image& Texture::image() const {
    return *pImpl->image;
}

const vk::ImageView& Texture::imageView() const {
    return *pImpl->imageView;
}

const vk::DescriptorSet& Texture::descriptorSet() const {
    return *pImpl->descriptorSet;
}

void Texture::replaceData(std::uint8_t* data) {
    auto& display = pImpl->display;
    auto& commandBuffer = display.commandBuffer();
    auto byteSize = m_size.width * m_size.height * pImpl->type.channels();

    vk::raii::Buffer stagingBuffer{display.device(), vk::BufferCreateInfo{}
            .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
            .setSize(byteSize)
            .setSharingMode(vk::SharingMode::eExclusive)};

    auto stagingMemory = display.allocMemory(stagingBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    auto mmap = stagingMemory.mapMemory(0, byteSize, {});
    std::memcpy(mmap, data, byteSize);
    stagingMemory.unmapMemory();

    auto bufferImageCopy = vk::BufferImageCopy{}
        .setImageSubresource(baseLayers)
        .setImageExtent({m_size.width, m_size.height, 1})
        .setBufferOffset(0);

    auto barrierU2T = vk::ImageMemoryBarrier{}
        .setImage(*pImpl->image)
        .setSubresourceRange(baseRange)
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
        .setSrcAccessMask(vk::AccessFlagBits::eNone)
        .setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

    auto barrierT2S = vk::ImageMemoryBarrier{}
        .setImage(*pImpl->image)
        .setSubresourceRange(baseRange)
        .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
        .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
        .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

    commandBuffer.reset({});
    commandBuffer.begin(vk::CommandBufferBeginInfo{}.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer,
                {}, {}, {}, {barrierU2T});
    commandBuffer.copyBufferToImage(*stagingBuffer, *pImpl->image, vk::ImageLayout::eTransferDstOptimal, {bufferImageCopy});
    commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
                {}, {}, {}, {barrierT2S});
    commandBuffer.end();

    display.queue().submit(vk::SubmitInfo{}.setCommandBuffers(commandBuffer), nullptr);
    display.queue().waitIdle();
}

}
