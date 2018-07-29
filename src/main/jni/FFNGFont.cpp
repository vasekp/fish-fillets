#include <assert.h>
#include "jnix.h"
#include "FFNGFont.h"

TTF_Font::TTF_Font(const char *file, int height_)
: height(height_)
, typeface(NULL)
{
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGFont");
	jmethodID mid = javaEnv->GetStaticMethodID(cls, "createFont", "(Ljava/lang/String;I)Lcz/ger/ffng/FFNGFont;");
	//__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "TTF_Font::TTF_Font 1 %p %p %p", javaEnv, cls, mid);

	if (mid == NULL) {
		assert("method not found");
		return;
	}

	jstring fileString = javaEnv->NewStringUTF(file);

    typeface = javaEnv->CallStaticObjectMethod(cls, mid, fileString, height_);

    javaEnv->DeleteLocalRef(cls);
    javaEnv->DeleteLocalRef(fileString);
}

TTF_Font::~TTF_Font() {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();

	// could stay empty
	if (typeface) {
		javaEnv->DeleteLocalRef(typeface);
		typeface = NULL;
	}
}

int TTF_Font::fontHeight() {
	//TODO get height from real font?
	// could return height for simplicity
	return height;
}

int TTF_Font::init() {
	// TODO prepare for rendering
	// could stay empty
	return 0;
}

void TTF_Font::quit() {
	// TODO release rendering
	// could stay empty
}

int TTF_Font::getWidth(const char *text) {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGFont");
	jmethodID mid = javaEnv->GetMethodID(cls, "getWidth", "(Ljava/lang/String;)I");
	//__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "TTF_Font::getWidth 1 %p %p %p", javaEnv, cls, mid);

	if (mid == NULL) {
		assert("method not found");
		return 0;
	}

	jstring textString = javaEnv->NewStringUTF(text);

    int result = javaEnv->CallIntMethod(typeface, mid, textString);

    javaEnv->DeleteLocalRef(cls);
    javaEnv->DeleteLocalRef(textString);

    return result;
}

int TTF_Font::getHeight(const char *text) {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGFont");
	jmethodID mid = javaEnv->GetMethodID(cls, "getHeight", "(Ljava/lang/String;)I");
	//__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "TTF_Font::getHeight 1 %p %p %p", javaEnv, cls, mid);

	if (mid == NULL) {
		assert("method not found");
		return 0;
	}

	jstring textString = javaEnv->NewStringUTF(text);

    int result = javaEnv->CallIntMethod(typeface, mid, textString);

    javaEnv->DeleteLocalRef(cls);
    javaEnv->DeleteLocalRef(textString);

    return result;
}

void TTF_Font::sizeUTF8(const char *text, int *width, int *height) {
	if (width) *width = getWidth(text);
	if (height) *height = getHeight(text);
}

SDL_Surface* TTF_Font::renderUTF8_Shaded(const char *text, Color fontColor, Color bgColor, float outlineWidth) {
	// bgColor could be ignored as it is transparent anyway
	return new SDL_Surface(typeface, text, fontColor.get32(), bgColor.get32(), outlineWidth);
}
