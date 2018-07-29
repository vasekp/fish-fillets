#include "jnix.h"
#include "FFNGInputEvent.h"

std::string FFNGInputEvent::getKeyName(SDLKey sym) {
	// TODO key names
	return "key";
}

int getTouchX() {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = JNI::getInstance()->getJavaCls();
	jobject obj = JNI::getInstance()->getJavaObj();
	jmethodID mid = javaEnv->GetMethodID(cls, "getTouchX", "()I");
	//__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "getTouchX 1 %p %p %p", javaEnv, cls, mid);

    return javaEnv->CallIntMethod(obj, mid);
}

int getTouchY() {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = JNI::getInstance()->getJavaCls();
	jobject obj = JNI::getInstance()->getJavaObj();
	jmethodID mid = javaEnv->GetMethodID(cls, "getTouchY", "()I");
	//__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "getTouchY 1 %p %p %p", javaEnv, cls, mid);

    return javaEnv->CallIntMethod(obj, mid);
}

Uint8 FFNGInputEvent::getTouch(int *x, int *y) {
	// TODO get touch location through JNI
	*x = getTouchX();
	*y = getTouchY();
	return (*x != -1 && *y != -1) ? SDL_BUTTON(SDL_BUTTON_LEFT) : 0; //bit 1 = LEFT, bit 2 = middle, bit 3 = right - see SDL_BUTTON
}

jobject getNextEvent() {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = JNI::getInstance()->getJavaCls();
	jobject obj = JNI::getInstance()->getJavaObj();
	jmethodID mid = javaEnv->GetMethodID(cls, "pollEvent", "()Lcz/ger/ffng/FFNGInputEvent;");
	//__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "getNextEvent 1 %p %p %p", javaEnv, cls, mid);

    return javaEnv->CallObjectMethod(obj, mid);
}

bool FFNGInputEvent::pollEvent(SDL_Event *event) {
	jobject ev = getNextEvent();

	if (ev != NULL) {
		event->type = JNI::getInstance()->getInt(ev, "type");
		switch(event->type) {
			case SDL_Event::MOUSEBUTTONDOWN:
				event->button.x = (int)JNI::getInstance()->getFloat(ev, "x");
				event->button.y = (int)JNI::getInstance()->getFloat(ev, "y");
				event->button.button = SDL_BUTTON(SDL_BUTTON_LEFT);
				break;
			case SDL_Event::KEYDOWN:
				event->key.sym = SDLKey(JNI::getInstance()->getInt(ev, "key"));
				FFNGInputEvent::pressed[event->key.sym] = 1;
				break;
			case SDL_Event::KEYUP:
				event->key.sym = SDLKey(JNI::getInstance()->getInt(ev, "key"));
				FFNGInputEvent::pressed[event->key.sym] = 0;
				break;
		}

		JNI::getInstance()->getJavaEnv()->DeleteLocalRef(ev);
		return true;
	}

	JNI::getInstance()->getJavaEnv()->DeleteLocalRef(ev); // NULL?
	return false;
}

Uint8 FFNGInputEvent::pressed[SDLK_LAST];

Uint8* FFNGInputEvent::getKeyState(int *numkeys) {
	for (int i = 0; i < SDLK_LAST; i++) {
		FFNGInputEvent::pressed[i] = 0;
	}
	return FFNGInputEvent::pressed;
}
