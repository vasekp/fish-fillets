#ifdef __cplusplus
#define FF_ENUM_CLASS class
extern "C" {
#else
#define FF_ENUM_CLASS
#endif

void* start(void* metalLayer);

enum : int {
    kTouchEventPointerDown,
    kTouchEventPointerUp,
    kTouchEventPointerMove,
    kTouchEventCancel,
    kTouchEventTwoTap
};

void touchEvent(void* pGlue, int type, float x, float y);

void renderAudio(void* pGlue, unsigned count, void* buffer);

#ifdef __cplusplus
}
#endif
