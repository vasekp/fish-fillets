#ifndef FISH_FILLETS_GRAPHICS_PROGRAM_H
#define FISH_FILLETS_GRAPHICS_PROGRAM_H

class DrawTarget;

class BaseProgram {
protected:
#ifdef FISH_FILLETS_USE_VULKAN
    //TODO
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

public:
#ifdef FISH_FILLETS_USE_VULKAN
    // TODO
#else
    BaseProgram(const ogl::Program& native) : m_native(native) { }
#endif

    void run(DrawTarget& target, const Params& params, Shape shape) const;

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
