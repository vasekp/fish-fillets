#include "subsystem/graphics.h"

void GraphicsSystem::resizeBuffers() {
#ifdef FISH_FILLETS_USE_VULKAN
    m_display.recreateSwapchain();
#endif
    auto size = m_display.size();
    m_offscreenTarget.resize(size.x, size.y);
    auto scale = 1 / m_graphics.coords(Graphics::CoordSystems::base).scale;
    m_blurTargets[0].resize(size.x, size.y, scale);
    m_blurTargets[1].resize(size.x, size.y, scale);
}

void GraphicsSystem::newFrame() {
#ifdef FISH_FILLETS_USE_VULKAN
    const auto& device = m_display.device();
    constexpr std::uint64_t noTimeout = std::numeric_limits<std::uint64_t>::max();
    if(auto ret = device.waitForFences(*inFlightFence, true, noTimeout); ret != vk::Result::eSuccess)
        Log::fatal("Error waiting for new frame");
    device.resetFences(*inFlightFence);
    std::tie(std::ignore, m_curImageIndex) = m_display.swapchain().acquireNextImage(noTimeout, *imageAvailableSemaphore);
    m_curImage = &m_display.swapchainImages()[m_curImageIndex];

    const auto& commandBuffer = m_display.commandBuffer();
    commandBuffer.reset({});
    commandBuffer.begin(vk::CommandBufferBeginInfo{}
            .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    static constexpr vk::ClearValue clearColor{{0.f, 0.f, 0.f, 1.f}};
    commandBuffer.beginRenderPass(vk::RenderPassBeginInfo{}
                .setRenderPass(m_display.renderPass())
                .setFramebuffer(m_offscreenTarget.framebuffer())
                .setRenderArea({{}, {m_display.width(), m_display.height()}})
                .setClearValues(clearColor),
            vk::SubpassContents::eInline);
    vk::Viewport viewport{0.f, 0.f, (float)m_display.width(), (float)m_display.height(), 0.f, 1.f};
    vk::Rect2D scissor{{}, {m_display.width(), m_display.height()}};
    commandBuffer.setViewport(0, viewport);
    commandBuffer.setScissor(0, scissor);
    //commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_prog);
    //commandBuffer.draw(4, 1, 0, 0);
#else
    m_offscreenTarget.bind();
    glClear(GL_COLOR_BUFFER_BIT);
#endif
}

void GraphicsSystem::bind(DrawTarget* target) {
#ifdef FISH_FILLETS_USE_VULKAN
    // TODO
#else
    if(target != m_curTarget)
        target->bind();
#endif
}

void GraphicsSystem::setViewport(ICoords origin, ICoords size) {
#ifdef FISH_FILLETS_USE_VULKAN
    // TODO
#else
    m_display.setViewport(origin, size);
#endif
}

void GraphicsSystem::setScissor(ICoords from, ICoords to) {
#ifdef FISH_FILLETS_USE_VULKAN
    // TODO
#else
    ICoords size = to - from;
    glScissor(from.x, from.y, size.x, size.y);
    glEnable(GL_SCISSOR_TEST);
#endif
}

void GraphicsSystem::releaseScissor() {
#ifdef FISH_FILLETS_USE_VULKAN
    // TODO
#else
    glDisable(GL_SCISSOR_TEST);
#endif
}

void GraphicsSystem::present(TextureTarget& target) {
#ifdef FISH_FILLETS_USE_VULKAN
    const auto& commandBuffer = m_display.commandBuffer();
    commandBuffer.endRenderPass();

    auto frameU2Tbarrier = vk::ImageMemoryBarrier{}
        .setImage(*m_curImage)
        .setSubresourceRange(vulkan::baseRange)
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
        .setSrcAccessMask(vk::AccessFlagBits::eNone)
        .setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
    auto frameT2Pbarrier = vk::ImageMemoryBarrier{}
        .setImage(*m_curImage)
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
    commandBuffer.blitImage(m_offscreenTarget.texture().native(), vk::ImageLayout::eTransferSrcOptimal, *m_curImage, vk::ImageLayout::eTransferDstOptimal, {imageBlit}, vk::Filter::eNearest);
    commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe,
            {}, {}, {}, {frameT2Pbarrier, offscreenT2Sbarrier});
    commandBuffer.end();

    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eTopOfPipe; // TODO "Subpass Dependencies"
    auto submitInfo = vk::SubmitInfo{}
            .setWaitSemaphores(*imageAvailableSemaphore)
            .setWaitDstStageMask(waitStage)
            .setCommandBuffers(commandBuffer)
            .setSignalSemaphores(*renderFinishedSemaphore);
    m_display.queue().submit(submitInfo, *inFlightFence);
    try {
        auto res = m_display.queue().presentKHR(vk::PresentInfoKHR{}
                .setWaitSemaphores(*renderFinishedSemaphore)
                .setSwapchains(*m_display.swapchain())
                .setImageIndices(m_curImageIndex));
        if(res == vk::Result::eSuboptimalKHR)
            Log::debug<Log::graphics>("Image presented on suboptimal surface");
    } catch(const std::exception& e) {
        Log::error("Error presenting image");
    }
    m_display.device().waitIdle();
#else
    DisplayTarget{*this, m_display}.draw(m_offscreenTarget.texture(), m_shaders.copy(), m_graphics.coords(Graphics::CoordSystems::null));
    m_display.swap();
#endif
}
