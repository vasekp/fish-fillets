#ifndef FISH_FILLETS_GRAPHICS_PROGRAM_H
#define FISH_FILLETS_GRAPHICS_PROGRAM_H

class DrawTarget;

class BaseProgram {
protected:
#ifdef FISH_FILLETS_USE_VULKAN
    const vulkan::Program& m_native;
#else
    const ogl::Program& m_native;
#endif

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
    struct PushConstants {
        std::array<float, 2> uSrcOffset;
        std::array<float, 2> uSrcSize;
        std::array<float, 2> uDstOffset;
        std::array<float, 2> uDstSize;
        alignas(16) std::array<float, 3> uCoords;
        alignas(8) std::array<float, 2> uSigns;
        alignas(16) std::array<float, 4> uColor; // TODO flat
    };
#endif

#ifdef FISH_FILLETS_USE_VULKAN
    BaseProgram(const vulkan::Program& native) : m_native(native) { }
#else
    BaseProgram(const ogl::Program& native) : m_native(native) { }
#endif

    void run(GraphicsSystem& system, DrawTarget& target, const Params& params, Shape shape) const;

protected:
    virtual void own_params() const { }
};

template<typename SpecParams>
class Program : public BaseProgram {
    SpecParams m_params;

public:
#ifdef FISH_FILLETS_USE_VULKAN
    Program(SpecParams params) : BaseProgram(), m_params(params) { }
#else
    Program(ogl::Program& native, SpecParams params) : BaseProgram(native), m_params(params) { }
#endif

    SpecParams& params() { return m_params; }

private:
    void own_params() const override;
};

#endif //FISH_FILLETS_GRAPHICS_PROGRAM_H
