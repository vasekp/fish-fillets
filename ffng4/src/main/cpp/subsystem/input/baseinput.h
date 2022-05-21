#ifndef FISH_FILLETS_BASEINPUT_H
#define FISH_FILLETS_BASEINPUT_H

class Instance;

class BaseInput : public IInput {
    Instance& m_instance;

public:
    BaseInput(Instance& instance) : m_instance(instance) { }

    bool handleKeyDown(Key key) override;
    bool handleKeyUp(Key key) override;
    bool handlePointerDown(FCoords pos) override;
    bool handlePointerUp() override;
    bool handlePointerMove(FCoords pos) override;
};

#endif //FISH_FILLETS_BASEINPUT_H
