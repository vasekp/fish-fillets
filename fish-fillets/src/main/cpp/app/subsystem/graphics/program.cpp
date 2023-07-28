#include "subsystem/graphics.h"

void BaseProgram::run([[maybe_unused]] GraphicsSystem& system, DrawTarget& target, const BaseProgram::Params& params, Shape shape) const {
#ifdef FISH_FILLETS_USE_VULKAN
    const auto& display = system.display();
    const auto& commandBuffer = display.commandBuffer();

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_native);

    BasePushConstants constants{
        {params.src.fx(), params.src.fy()},
        {params.srcSize.fx(), params.srcSize.fy()},
        {params.dest.fx(), params.dest.fy()},
        {params.dstSize.fx(), params.dstSize.fy()},
        {params.area.fx(), params.area.fy()},
        {params.coords.origin.fx(), params.coords.origin.fy(), params.coords.scale}
    };
    commandBuffer.pushConstants<BasePushConstants>(m_native.pipelineLayout(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, constants);

    if(params.texture) {
        const auto& descriptorSet = params.texture->native().descriptorSet();
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_native.pipelineLayout(), 0, {descriptorSet}, {});
    }

    own_params(system);

    commandBuffer.draw(4, 1, 0, 0);
#else
    if(params.texture)
        params.texture->bind();
    glUseProgram(m_native);
    glUniform2f(m_native.uniform("uSrcSize"), params.srcSize.fx(), params.srcSize.fy());
    glUniform2f(m_native.uniform("uDstSize"), params.dstSize.fx(), params.dstSize.fy());
    glUniform2f(m_native.uniform("uSrcOffset"), params.src.fx(), params.src.fy());
    glUniform2f(m_native.uniform("uDstOffset"), params.dest.fx(), params.dest.fy());
    glUniform3f(m_native.uniform("uCoords"), params.coords.origin.fx(), params.coords.origin.fy(), params.coords.scale);
    glUniform2f(m_native.uniform("uSigns"), 1.f, params.flipY ? -1.f : 1.f);

    own_params();

    switch(shape) {
        case Shape::rect: {
            float vertices[4][2] = {
                {0, 0},
                {0, params.area.fy()},
                {params.area.fx(), 0},
                {params.area.fx(), params.area.fy()}
            };
            glVertexAttribPointer(ogl::Program::aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &vertices[0][0]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        case Shape::triangle: {
            float vertices[3][3] = {
                {1, 0, 0},
                {0, 1, 0},
                {0, 0, 1}
            };
            glVertexAttribPointer(ogl::Program::aPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), &vertices[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }
#endif
}
