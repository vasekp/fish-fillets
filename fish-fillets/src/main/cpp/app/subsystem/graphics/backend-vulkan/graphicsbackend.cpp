#include "subsystem/graphics.h/graphicsbackend.h"

struct GraphicsBackend::VulkanDetail {
    vk::raii::Semaphore imageAvailableSemaphore;
    vk::raii::Semaphore renderFinishedSemaphore;
    vk::raii::Fence inFlightFence;

    std::uint32_t curImageIndex;
    const vk::Image* curImage;
};

std::unique_ptr<GraphicsBackend::VulkanDetail> GraphicsBackend::vulkanDetail() {
    const auto& device = m_display.device();

    return std::make_unique<VulkanDetail>(
        vk::raii::Semaphore{device, vk::SemaphoreCreateInfo{}},
        vk::raii::Semaphore{device, vk::SemaphoreCreateInfo{}},
        vk::raii::Fence{device, vk::FenceCreateInfo{}.setFlags(vk::FenceCreateFlagBits::eSignaled)},
        0,
        nullptr
    );
}

GraphicsBackend::GraphicsBackend(Instance& instance, BACKEND::Display&& display) :
    m_graphics(instance.graphics()),
    m_display(std::move(display)),
    m_blurTargets{{
        TextureTarget{*this, blurTargetDims(), FCoords{m_display.size()}},
        TextureTarget{*this, blurTargetDims(), FCoords{m_display.size()}}}},
    m_offscreenTarget{*this, m_display.size()},
    m_shaders{instance, *this}
{
    m_detail = vulkanDetail();
}

GraphicsBackend::~GraphicsBackend() = default;

void GraphicsBackend::resizeBuffers() {
    m_display.recreateSwapchain();
    auto dispSize = m_display.size();
    m_offscreenTarget.resize(dispSize);
    auto blurSize = blurTargetDims();
    m_blurTargets[0].resize(blurSize, FCoords{dispSize});
    m_blurTargets[1].resize(blurSize, FCoords{dispSize});
}

USize GraphicsBackend::blurTargetDims() {
    auto dispSize = FCoords{m_display.size()};
    auto scale = std::max(Graphics::baseDim.x / dispSize.x, Graphics::baseDim.y / dispSize.y);
    auto size = (scale * dispSize).toSize();
    Log::debug<Log::graphics>("Blur dims: ", size);
    return size;
}

void GraphicsBackend::newFrame() {
    const auto& device = m_display.device();
    constexpr std::uint64_t noTimeout = std::numeric_limits<std::uint64_t>::max();
    if(auto ret = device.waitForFences(*m_detail->inFlightFence, true, noTimeout); ret != vk::Result::eSuccess)
        Log::fatal("Error waiting for new frame");
    device.resetFences(*m_detail->inFlightFence);
    std::tie(std::ignore, m_detail->curImageIndex) = m_display.swapchain().acquireNextImage(noTimeout, *m_detail->imageAvailableSemaphore);
    m_detail->curImage = &m_display.swapchainImages()[m_detail->curImageIndex];

    static constexpr auto clearColor = vk::ClearValue{{0.f, 0.f, 0.f, 1.f}};
    static constexpr auto clearAttachment = vk::ClearAttachment{}
        .setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setColorAttachment(0)
        .setClearValue(clearColor);
    const auto& commandBuffer = m_display.commandBuffer();
    commandBuffer.reset({});
    commandBuffer.begin(vk::CommandBufferBeginInfo{}
            .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    commandBuffer.beginRenderPass(vk::RenderPassBeginInfo{}
                .setRenderPass(m_display.renderPass())
                .setFramebuffer(m_offscreenTarget.framebuffer())
                .setRenderArea({{}, {m_display.width(), m_display.height()}}),
            vk::SubpassContents::eInline);
    vk::Viewport viewport{0.f, 0.f, (float)m_display.width(), (float)m_display.height(), 0.f, 1.f};
    vk::Rect2D fullRect{{}, {m_display.width(), m_display.height()}};
    commandBuffer.setViewport(0, viewport);
    commandBuffer.setScissor(0, fullRect);
    vk::ClearRect clearRect{fullRect, 0, 1};
    commandBuffer.clearAttachments(clearAttachment, clearRect);
    m_curTarget = &m_offscreenTarget;
}

void GraphicsBackend::bind(DrawTarget* target) {
    if(target == m_curTarget)
        return;
    const auto& commandBuffer = m_display.commandBuffer();
    commandBuffer.endRenderPass();

    TextureTarget* ttarget = dynamic_cast<TextureTarget*>(target);
    auto size = FCoords{ttarget->texture().physSize()};
    commandBuffer.beginRenderPass(vk::RenderPassBeginInfo{}
                .setRenderPass(m_display.renderPass())
                .setFramebuffer(ttarget->framebuffer())
                .setRenderArea({{}, {(std::uint32_t)size.x, (std::uint32_t)size.y}}),
            vk::SubpassContents::eInline);
    vk::Viewport viewport{0.f, 0.f, size.x, size.y, 0.f, 1.f};
    vk::Rect2D fullRect{{}, {(std::uint32_t)size.x, (std::uint32_t)size.y}};
    commandBuffer.setViewport(0, viewport);
    commandBuffer.setScissor(0, fullRect);
    m_curTarget = target;
}

void GraphicsBackend::setViewport(ICoords origin, ICoords size) {
    // not used in Vulkan
}

void GraphicsBackend::setScissor(ICoords from, ICoords to) {
    ICoords size = to - from;
    const auto& commandBuffer = m_display.commandBuffer();
    vk::Rect2D rect{{from.x, from.y}, {(std::uint32_t)size.x, (std::uint32_t)size.y}};
    commandBuffer.setScissor(0, rect);
}

void GraphicsBackend::releaseScissor() {
    const auto& commandBuffer = m_display.commandBuffer();
    vk::Rect2D fullRect{{}, {m_display.width(), m_display.height()}};
    commandBuffer.setScissor(0, fullRect);
}

void GraphicsBackend::present(TextureTarget& target) {
    const auto& commandBuffer = m_display.commandBuffer();
    commandBuffer.endRenderPass();

    auto frameU2Tbarrier = vk::ImageMemoryBarrier{}
        .setImage(*m_detail->curImage)
        .setSubresourceRange(vulkan::baseRange)
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
        .setSrcAccessMask(vk::AccessFlagBits::eNone)
        .setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
    auto frameT2Pbarrier = vk::ImageMemoryBarrier{}
        .setImage(*m_detail->curImage)
        .setSubresourceRange(vulkan::baseRange)
        .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
        .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
        .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
        .setDstAccessMask(vk::AccessFlagBits::eMemoryRead);

    auto offscreenS2Tbarrier = vk::ImageMemoryBarrier{}
        .setImage(m_offscreenTarget.texture().native())
        .setSubresourceRange(vulkan::baseRange)
        .setOldLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
        .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
        .setDstAccessMask(vk::AccessFlagBits::eTransferRead);
    auto offscreenT2Sbarrier = vk::ImageMemoryBarrier{}
        .setImage(m_offscreenTarget.texture().native())
        .setSubresourceRange(vulkan::baseRange)
        .setOldLayout(vk::ImageLayout::eTransferSrcOptimal)
        .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setSrcAccessMask(vk::AccessFlagBits::eTransferRead)
        .setDstAccessMask(vk::AccessFlagBits::eNone);

    std::array<vk::Offset3D, 2> rect{{{}, {(std::int32_t)m_display.width(), (std::int32_t)m_display.height(), 1}}};
    auto imageBlit = vk::ImageBlit{}
        .setSrcSubresource(vulkan::baseLayers)
        .setSrcOffsets(rect)
        .setDstSubresource(vulkan::baseLayers)
        .setDstOffsets(rect);
    commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eTransfer,
            {}, {}, {}, {frameU2Tbarrier, offscreenS2Tbarrier});
    commandBuffer.blitImage(m_offscreenTarget.texture().native(), vk::ImageLayout::eTransferSrcOptimal, *m_detail->curImage, vk::ImageLayout::eTransferDstOptimal, {imageBlit}, vk::Filter::eNearest);
    commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe,
            {}, {}, {}, {frameT2Pbarrier, offscreenT2Sbarrier});
    commandBuffer.end();

    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eTopOfPipe;
    auto submitInfo = vk::SubmitInfo{}
            .setWaitSemaphores(*m_detail->imageAvailableSemaphore)
            .setWaitDstStageMask(waitStage)
            .setCommandBuffers(commandBuffer)
            .setSignalSemaphores(*m_detail->renderFinishedSemaphore);
    m_display.queue().submit(submitInfo, *m_detail->inFlightFence);
    try {
        auto res = m_display.queue().presentKHR(vk::PresentInfoKHR{}
                .setWaitSemaphores(*m_detail->renderFinishedSemaphore)
                .setSwapchains(*m_display.swapchain())
                .setImageIndices(m_detail->curImageIndex));
        if(res == vk::Result::eSuboptimalKHR)
            Log::debug<Log::graphics>("Image presented on suboptimal surface");
    } catch(const std::exception& e) {
        Log::error("Error presenting image");
    }
    m_display.device().waitIdle();
}
