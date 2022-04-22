#include "assert.h"
#include "FFNGVideo.h"
#include <android/log.h>
#include <android/bitmap.h>

SDL_Surface* FFNGVideo::setVideoMode(int width, int height, int bpp, int flags) {
	// ignore bpp and flags
    __android_log_print(ANDROID_LOG_DEBUG, "FFNG", "FFNGVideo::setVideoMode %d %d", width, height);
    JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
    jclass cls = JNI::getInstance()->getJavaCls();
    jobject obj = JNI::getInstance()->getJavaObj();
    jmethodID mid = javaEnv->GetMethodID(cls, "setWindowSize", "(II)V");
	//__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "FFNGVideo::setVideoMode 1 %p %p %p", javaEnv, cls, mid);

	if (mid == NULL) {
		assert("method not found");
		return NULL;
	}

    javaEnv->CallVoidMethod(obj, mid, width, height);

	return new SDL_Surface(width, height);
}

void FFNGVideo::setWindowCaption(const std::string &caption) {
    JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
    jclass cls = JNI::getInstance()->getJavaCls();
    jobject obj = JNI::getInstance()->getJavaObj();
    jmethodID mid = javaEnv->GetMethodID(cls, "setWindowCaption", "(Ljava/lang/String;)V");
	//__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "FFNGVideo::setWindowCaption 1 %p %p %p", javaEnv, cls, mid);

	if (mid == NULL) {
		assert("method not found");
		return;
	}

	jstring captionString = javaEnv->NewStringUTF(caption.c_str());
    javaEnv->CallVoidMethod(obj, mid, captionString);

	javaEnv->DeleteLocalRef(captionString);
}

void FFNGVideo::flip(SDL_Surface *screen) {
    JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
    jclass cls = JNI::getInstance()->getJavaCls();
    jobject obj = JNI::getInstance()->getJavaObj();
    jmethodID mid = javaEnv->GetMethodID(cls, "renderThis", "()V");
	//__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "FFNGVideo::flip 1 %p %p %p", javaEnv, cls, mid);
	
	if (mid == NULL) {
		assert("method not found");
		return;
	}

	jfieldID fid = javaEnv->GetFieldID(cls, "bmp", "Landroid/graphics/Bitmap;");
	jobject bmp = javaEnv->GetObjectField(obj, fid);

    glBindFramebuffer(GL_FRAMEBUFFER, FFNGSurface::framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen->texture, 0);

    eglSwapBuffers(FFNGSurface::dpy, FFNGSurface::sfc);

    void* pixels;
    AndroidBitmap_lockPixels(javaEnv, bmp, &pixels);
	glReadPixels(0, 0, MaxWidth, MaxHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	AndroidBitmap_unlockPixels(javaEnv, bmp);
	javaEnv->DeleteLocalRef(bmp);

	javaEnv->CallVoidMethod(obj, mid, screen->getTexture());
}
