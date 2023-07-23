#include "../vulkan.h"

namespace vulkan {

struct TextureImpl {
    const vk::raii::Image image;
    const vk::raii::DeviceMemory memory;
    const vk::raii::ImageView imageView;
    //const vk::raii::Sampler sampler;
};

Texture::Texture(std::uint32_t width, std::uint32_t height, vk::raii::Image&& image, vk::raii::DeviceMemory&& memory,
        vk::raii::ImageView&& imageView/*, vk::raii::Sampler&& sampler*/) :
    pImpl{std::make_unique<TextureImpl>(
        std::move(image),
        std::move(memory),
        std::move(imageView)
        /*std::move(sampler)*/)},
    m_width(width), m_height(height)
{ }

Texture::Texture(Texture&&) = default;
Texture& Texture::operator=(Texture&&) = default;
Texture::~Texture() = default;

Texture Texture::empty(const Display& display, std::uint32_t width, std::uint32_t height) {
    return fromImageData(display, width, height, 4, nullptr);
}

Texture Texture::fromImageData(const Display& display, std::uint32_t width, std::uint32_t height, int channels, void *data) {
    vk::raii::Image image{display.device(), vk::ImageCreateInfo{}
        .setImageType(vk::ImageType::e2D)
        .setExtent({width, height, 1})
        .setMipLevels(1)
        .setArrayLayers(1)
        .setFormat(channels == 4 ? vk::Format::eR8G8B8A8Srgb : vk::Format::eR8Srgb)
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
            .setFormat(vk::Format::eR8G8B8A8Srgb)
            .setSubresourceRange(baseRange)};

    /*vk::raii::Sampler sampler{display.device(), vk::SamplerCreateInfo{}
            .setMagFilter(vk::Filter::eNearest)
            .setMinFilter(vk::Filter::eNearest)
            .setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
            .setAddressModeV(vk::SamplerAddressMode::eClampToEdge)
            .setAnisotropyEnable(vk::False)
            .setUnnormalizedCoordinates(vk::False)
            .setCompareEnable(vk::False)};*/

    auto& commandBuffer = display.commandBuffer();

    if(data) {
        auto byteSize = width * height * channels;

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
            .setImageExtent({3, 2, 1})
            .setBufferOffset(0);

        auto barrierU2T = vk::ImageMemoryBarrier{}
            .setImage(*image)
            .setSubresourceRange(baseRange)
            .setOldLayout(vk::ImageLayout::eUndefined)
            .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
            .setSrcAccessMask(vk::AccessFlagBits::eNone)
            .setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

        auto barrierT2S = vk::ImageMemoryBarrier{}
            .setImage(*image)
            .setSubresourceRange(baseRange)
            .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
            .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
            .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        commandBuffer.reset({});
        commandBuffer.begin(vk::CommandBufferBeginInfo{}.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer,
                    {}, {}, {}, {barrierU2T});
        commandBuffer.copyBufferToImage(*stagingBuffer, *image, vk::ImageLayout::eTransferDstOptimal, {bufferImageCopy});
        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
                    {}, {}, {}, {barrierT2S});
        commandBuffer.end();

        display.queue().submit(vk::SubmitInfo{}.setCommandBuffers(commandBuffer), nullptr);
        display.queue().waitIdle();
    } else {
        auto barrierU2S = vk::ImageMemoryBarrier{}
            .setImage(*image)
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

    return Texture{width, height, std::move(image), std::move(deviceMemory), std::move(imageView)/*, std::move(sampler)*/};
}

const vk::Image& Texture::image() const {
    return *pImpl->image;
}

const vk::ImageView& Texture::imageView() const {
    return *pImpl->imageView;
}

}
