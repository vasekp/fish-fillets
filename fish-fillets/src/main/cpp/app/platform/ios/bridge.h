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

void resizeWin(void* pGlue, unsigned width, unsigned height);
void setFocus(void* pGlue, int focus);
void touchEvent(void* pGlue, int type, float x, float y);

void renderAudio(void* pGlue, unsigned count, void* buffer);

#ifdef __cplusplus
}
#endif
