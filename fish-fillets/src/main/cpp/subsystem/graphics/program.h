#ifndef FISH_FILLETS_GRAPHICS_PROGRAM_H
#define FISH_FILLETS_GRAPHICS_PROGRAM_H

class DrawTarget;

class Program {
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

protected:
    Program(const ogl::Program& native) : m_native(native) { }

public:
    void run(DrawTarget& target, const Params& params) const;

protected:
    virtual void own_params() const = 0;
};

#endif //FISH_FILLETS_GRAPHICS_PROGRAM_H
