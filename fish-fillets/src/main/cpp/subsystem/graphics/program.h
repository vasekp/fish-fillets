#ifndef FISH_FILLETS_GRAPHICS_PROGRAM_H
#define FISH_FILLETS_GRAPHICS_PROGRAM_H

class DrawTarget;

class BaseProgram {
protected:
    const ogl::Program& m_native;

public:
    struct Params {
        std::optional<TextureView> image{};
        FCoords src{};
        FCoords dest{};
        FCoords srcSize{};
        FCoords dstSize{};
        FCoords area{};
        Coords coords{};
        bool flipY{false};
    };

public:
    BaseProgram(const ogl::Program& native) : m_native(native) { }

    void run(DrawTarget& target, const Params& params) const;

protected:
    virtual void own_params() const { }
};

template<typename SpecParams>
class Program : public BaseProgram {
    SpecParams m_params;

public:
    Program(ogl::Program& native, SpecParams params) : BaseProgram(native), m_params(params) { }

    SpecParams& params() { return m_params; }

private:
    void own_params() const override;
};

#endif //FISH_FILLETS_GRAPHICS_PROGRAM_H
