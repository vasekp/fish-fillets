#include "FFNGApp.h"
#include "jnix.h"
#include <assert.h>
#include <unistd.h>

int FFNGApp::getTicks() {
	// System.currentTimeMillis
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = JNI::getInstance()->getJavaCls();
	jobject obj = JNI::getInstance()->getJavaObj();
	jmethodID mid = javaEnv->GetMethodID(cls, "getTicks", "()I");
	//__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "FFNGApp::getTicks 1 %p %p %p", javaEnv, cls, mid);

	if (mid == NULL) {
		assert("method not found");
		return 0;
	}

    return javaEnv->CallIntMethod(obj, mid);
}

void FFNGApp::delay(Uint32 ms) {
	// pause game for a while (ms)
	usleep(ms * 1000);
}

void FFNGApp::setGameState(int newState) {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = JNI::getInstance()->getJavaCls();
	jobject obj = JNI::getInstance()->getJavaObj();
	jmethodID mid = javaEnv->GetMethodID(cls, "setGameState", "(I)V");
	//__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "FFNGApp::setGameState 1 %p %p %p", javaEnv, cls, mid);

	if (mid == NULL) {
		assert("method not found");
		return;
	}

    javaEnv->CallVoidMethod(obj, mid, newState);
}

int FFNGApp::getGameState() {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = JNI::getInstance()->getJavaCls();
	jobject obj = JNI::getInstance()->getJavaObj();
	jmethodID mid = javaEnv->GetMethodID(cls, "getGameState", "()I");
	//__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "FFNGApp::getGameState 1 %p %p %p", javaEnv, cls, mid);

	if (mid == NULL) {
		assert("method not found");
		return GAMESTATE_LOADING;
	}

    return javaEnv->CallIntMethod(obj, mid);
}

void FFNGApp::switchFishes(bool isBig) {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = JNI::getInstance()->getJavaCls();
	jobject obj = JNI::getInstance()->getJavaObj();
	jmethodID mid = javaEnv->GetMethodID(cls, "switchFishes", "(Z)V");
	//__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "FFNGApp::switchFishes 1 %p %p %p", javaEnv, cls, mid);

	if (mid == NULL) {
		assert("method not found");
		return;
	}

    javaEnv->CallVoidMethod(obj, mid, isBig);
}

void FFNGApp::saveEffect() {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = JNI::getInstance()->getJavaCls();
	jobject obj = JNI::getInstance()->getJavaObj();
	jmethodID mid = javaEnv->GetMethodID(cls, "saveEffect", "()V");
	//__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "FFNGApp::saveEffect 1 %p %p %p", javaEnv, cls, mid);

	if (mid == NULL) {
		assert("method not found");
		return;
	}

    javaEnv->CallVoidMethod(obj, mid);
}

bool FFNGApp::pauseAndDisposeChance() {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = JNI::getInstance()->getJavaCls();
	jobject obj = JNI::getInstance()->getJavaObj();
	jmethodID mid = javaEnv->GetMethodID(cls, "pauseAndDisposeChance", "()Z");
	//__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "FFNGApp::pauseAndDisposeChance 1 %p %p %p", javaEnv, cls, mid);

	if (mid == NULL) {
		assert("method not found");
		return true;
	}

    return javaEnv->CallBooleanMethod(obj, mid);
}
