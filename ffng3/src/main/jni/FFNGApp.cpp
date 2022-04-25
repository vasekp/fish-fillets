#include "FFNGApp.h"
#include "jnix.h"
#include "Log.h"
#include <unistd.h>
#include <stdexcept>

int FFNGApp::getTicks() {
	// System.currentTimeMillis
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = JNI::getInstance()->getJavaCls();
	jobject obj = JNI::getInstance()->getJavaObj();
	jmethodID mid = javaEnv->GetMethodID(cls, "getTicks", "()I");
	Log::debug("FFNGApp::getTicks 1 %p %p %p", javaEnv, cls, mid);

	if (!mid) {
		throw std::logic_error("method not found: getTicks");
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
	Log::debug("FFNGApp::setGameState 1 %p %p %p", javaEnv, cls, mid);

	if (!mid) {
		throw std::logic_error("method not found: setGameState");
	}

    javaEnv->CallVoidMethod(obj, mid, newState);
}

int FFNGApp::getGameState() {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = JNI::getInstance()->getJavaCls();
	jobject obj = JNI::getInstance()->getJavaObj();
	jmethodID mid = javaEnv->GetMethodID(cls, "getGameState", "()I");
	Log::debug("FFNGApp::getGameState 1 %p %p %p", javaEnv, cls, mid);

	if (!mid) {
		throw std::logic_error("method not found: getGameState");
	}

    return javaEnv->CallIntMethod(obj, mid);
}

void FFNGApp::switchFishes(bool isBig) {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = JNI::getInstance()->getJavaCls();
	jobject obj = JNI::getInstance()->getJavaObj();
	jmethodID mid = javaEnv->GetMethodID(cls, "switchFishes", "(Z)V");
	Log::debug("FFNGApp::switchFishes 1 %p %p %p", javaEnv, cls, mid);

	if (!mid) {
		throw std::logic_error("method not found: switchFishes");
	}

    javaEnv->CallVoidMethod(obj, mid, isBig);
}

void FFNGApp::saveEffect() {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = JNI::getInstance()->getJavaCls();
	jobject obj = JNI::getInstance()->getJavaObj();
	jmethodID mid = javaEnv->GetMethodID(cls, "saveEffect", "()V");
	Log::debug("FFNGApp::saveEffect 1 %p %p %p", javaEnv, cls, mid);

	if (!mid) {
		throw std::logic_error("method not found: saveEffect");
	}

    javaEnv->CallVoidMethod(obj, mid);
}

bool FFNGApp::pauseAndDisposeChance() {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = JNI::getInstance()->getJavaCls();
	jobject obj = JNI::getInstance()->getJavaObj();
	jmethodID mid = javaEnv->GetMethodID(cls, "pauseAndDisposeChance", "()Z");
	Log::debug("FFNGApp::pauseAndDisposeChance 1 %p %p %p", javaEnv, cls, mid);

	if (!mid) {
		throw std::logic_error("method not found: pauseAndDisposeChance");
	}

    return javaEnv->CallBooleanMethod(obj, mid);
}
