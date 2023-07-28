#ifndef FISH_FILLETS_GRAPHICS_PROGRAM_H
#define FISH_FILLETS_GRAPHICS_PROGRAM_H

class DrawTarget;

class BaseProgram {
protected:
#ifdef FISH_FILLETS_USE_VULKAN
    using PlatformType = vulkan::Program;
#else
    using PlatformType = ogl::Program;
#endif
    const PlatformType& m_native;

public:
    struct Params {
        const Texture* texture{};
        FCoords src{};
        FCoords dest{};
        FCoords srcSize{};
        FCoords dstSize{};
        FCoords area{};
        Coords coords{};
        bool flipY{false};
    };

    enum class Shape {
        rect,
        triangle
    };

#ifdef FISH_FILLETS_USE_VULKAN
    struct BasePushConstants {
        std::array<float, 2> uSrcOffset;
        std::array<float, 2> uSrcSize;
        std::array<float, 2> uDstOffset;
        std::array<float, 2> uDstSize;
        std::array<float, 2> uArea;
        alignas(16) std::array<float, 4> uCoords;
    };

    constexpr static auto basePushConstantSize = sizeof(BasePushConstants);
    constexpr static auto ownPushConstantOffset = basePushConstantSize;
#endif

    BaseProgram(const PlatformType& native) : m_native(native) { }

    void run(GraphicsSystem& system, DrawTarget& target, const Params& params, Shape shape) const;

protected:
    virtual void own_params(GraphicsSystem& system) const { }
};

template<typename SpecParams>
class Program : public BaseProgram {
    SpecParams m_params;

public:
    Program(const PlatformType& native, SpecParams params) : BaseProgram(native), m_params(params) { }

    SpecParams& params() { return m_params; }

private:
    void own_params(GraphicsSystem& system) const override;
};

#endif //FISH_FILLETS_GRAPHICS_PROGRAM_H
