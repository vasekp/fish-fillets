#ifndef FISH_FILLETS_BASEINPUT_H
#define FISH_FILLETS_BASEINPUT_H

class Instance;

class BaseInput : public IInputSink {
    Instance& m_instance;

public:
    BaseInput(Instance& instance) : m_instance(instance) { }

    bool keyDown(Key key) override;
    bool pointerDown(FCoords coords) override;
};

#endif //FISH_FILLETS_BASEINPUT_H
