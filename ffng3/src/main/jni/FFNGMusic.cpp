#include <stdexcept>
#include "FFNGMusic.h"
#include "Log.h"

void FFNGMusic::openAudio(int frequency) {
	// could stay empty
}

void FFNGMusic::closeAudio() {
	// could stay empty
}

Mix_Chunk* FFNGMusic::loadWAV(const char *file) {
	return new Mix_Chunk(file);
}

void FFNGMusic::halt(int channel) {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGSound");
	jmethodID mid = javaEnv->GetStaticMethodID(cls, "halt", "(I)V");
	Log::debug("FFNGMusic::halt 1 %p %p %p", javaEnv, cls, mid);

	if(!mid) {
		throw std::logic_error("method not found: halt");
	}

	javaEnv->CallStaticVoidMethod(cls, mid, channel);

    javaEnv->DeleteLocalRef(cls);
}

bool FFNGMusic::isPlaying(int channel) {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGSound");
	jmethodID mid = javaEnv->GetStaticMethodID(cls, "isPlaying", "(I)Z");
	Log::debug("FFNGMusic::isPlaying 1 %p %p %p", javaEnv, cls, mid);

	if(!mid) {
		throw std::logic_error("method not found: isPlaying");
	}

	bool ret = javaEnv->CallStaticBooleanMethod(cls, mid, channel);

    javaEnv->DeleteLocalRef(cls);
    return ret;
}

int FFNGMusic::playChannel(int channel, Mix_Chunk *sound, int loops) {
	// TODO don't forget the loops
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGSound");
	jmethodID mid = javaEnv->GetMethodID(cls, "playChannel", "(II)I");
	Log::debug("FFNGMusic::playChannel 1 %p %p %p", javaEnv, cls, mid);

	if(!mid) {
		throw std::logic_error("method not found: playChannel");
	}

	int ch = javaEnv->CallIntMethod(sound->getSoundObject(), mid, channel, loops);
    sound->setChannel(ch);

    javaEnv->DeleteLocalRef(cls);
    return ch;
}

void FFNGMusic::volume(int channel, float vol) {
	// TODO set channel volume, -1 for all
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGSound");
	jmethodID mid = javaEnv->GetStaticMethodID(cls, "volume", "(IF)V");
	Log::debug("FFNGMusic::volume 1 %p %p %p", javaEnv, cls, mid);

	if(!mid) {
		throw std::logic_error("method not found: volume");
	}

	javaEnv->CallStaticVoidMethod(cls, mid, channel, vol);

    javaEnv->DeleteLocalRef(cls);
}

void FFNGMusic::free(Mix_Chunk *chunk) {
	if (chunk) {
		delete chunk;
		chunk = NULL;
	}
}

Mix_Music* FFNGMusic::loadMUS(const char *file) {
	return new Mix_Music(file);
}

int FFNGMusic::playMusic(Mix_Music* music, int loops) {
	// TODO loops, return value (channel?)
	music->play();
	return 0;
}

void FFNGMusic::volumeMusic(int vol) {
	Mix_Music::setVolumeAll(vol);
}

bool FFNGMusic::playingMusic() {
	return false;
}

void FFNGMusic::haltMusic() {
	Mix_Music::stopAll();
}

void FFNGMusic::freeMusic(Mix_Music *music) {
	music->dispose();
	if (music) {
		delete music;
		music = NULL;
	}
}


Mix_Music::Mix_Music(const char *file) {
	music = loadMusic(file);
}

Mix_Music::~Mix_Music() {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGMusic");
	jmethodID mid = javaEnv->GetMethodID(cls, "dispose", "()V");
	Log::debug("Mix_Music::~Mix_Music 1 %p %p %p", javaEnv, cls, mid);

	if(!mid) {
		Log::error("method not found: dispose");
	}

    javaEnv->CallVoidMethod(music, mid);

    javaEnv->DeleteLocalRef(cls);
	javaEnv->DeleteLocalRef(music);
}

jobject Mix_Music::loadMusic(const char *file) {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGMusic");
	jmethodID mid = javaEnv->GetStaticMethodID(cls, "loadMusic", "(Ljava/lang/String;)Lcz/ger/ffng/FFNGMusic;");
	Log::debug("Mix_Music::loadMusic 1 %p %p %p", javaEnv, cls, mid);

	if(!mid) {
		throw std::logic_error("method not found: loadMusic");
	}

	jstring fileString = javaEnv->NewStringUTF(file);

    jobject result = javaEnv->CallStaticObjectMethod(cls, mid, fileString);

    javaEnv->DeleteLocalRef(cls);
    javaEnv->DeleteLocalRef(fileString);

    return result;
}

void Mix_Music::play() {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGMusic");
	jmethodID mid = javaEnv->GetMethodID(cls, "play", "()V");
	Log::debug("Mix_Music::play 1 %p %p %p", javaEnv, cls, mid);

	if(!mid) {
		throw std::logic_error("method not found: play");
	}

    javaEnv->DeleteLocalRef(cls);
    javaEnv->CallVoidMethod(music, mid);
}

void Mix_Music::stop() {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGMusic");
	jmethodID mid = javaEnv->GetMethodID(cls, "stop", "()V");
	Log::debug("Mix_Music::stop 1 %p %p %p", javaEnv, cls, mid);

	if(!mid) {
		throw std::logic_error("method not found: stop");
	}

    javaEnv->DeleteLocalRef(cls);
    javaEnv->CallVoidMethod(music, mid);
}

void Mix_Music::dispose() {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGMusic");
	jmethodID mid = javaEnv->GetMethodID(cls, "dispose", "()V");
	Log::debug("Mix_Music::dispose 1 %p %p %p", javaEnv, cls, mid);

	if(!mid) {
		throw std::logic_error("method not found: dispose");
	}

    javaEnv->DeleteLocalRef(cls);
    javaEnv->CallVoidMethod(music, mid);
}

void Mix_Music::stopAll() {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGMusic");
	jmethodID mid = javaEnv->GetStaticMethodID(cls, "stopAll", "()V");
	Log::debug("Mix_Music::stopAll 1 %p %p %p", javaEnv, cls, mid);

	if(!mid) {
		throw std::logic_error("method not found: stopAll");
	}

    javaEnv->CallStaticVoidMethod(cls, mid);
    javaEnv->DeleteLocalRef(cls);
}

void Mix_Music::setVolumeAll(int vol) {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGMusic");
	jmethodID mid = javaEnv->GetStaticMethodID(cls, "setVolumeAll", "(I)V");
	Log::debug("Mix_Music::setvolumeAll 1 %p %p %p", javaEnv, cls, mid);

	if(!mid) {
		throw std::logic_error("method not found: setVolumeAll");
	}

    javaEnv->CallStaticVoidMethod(cls, mid, vol);
    javaEnv->DeleteLocalRef(cls);
}

Mix_Chunk::Mix_Chunk(const char *file)
: channel(-1)
{
	sound = loadSound(file);
}

Mix_Chunk::~Mix_Chunk() {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGSound");
	jmethodID mid = javaEnv->GetMethodID(cls, "dispose", "()V");
	Log::debug("Mix_Chunk::~Mix_Chunk 1 %p %p %p", javaEnv, cls, mid);

	if(!mid) {
		Log::error("method not found: dispose");
	}

    javaEnv->CallVoidMethod(sound, mid);

    javaEnv->DeleteLocalRef(cls);
	javaEnv->DeleteLocalRef(sound);
}

jobject Mix_Chunk::loadSound(const char *file) {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGSound");
	jmethodID mid = javaEnv->GetStaticMethodID(cls, "loadSound", "(Ljava/lang/String;)Lcz/ger/ffng/FFNGSound;");
	Log::debug("Mix_Chunk::loadSound 1 %p %p %p", javaEnv, cls, mid);

	if(!mid) {
		throw std::logic_error("method not found: loadSound");
	}

	jstring fileString = javaEnv->NewStringUTF(file);

    jobject result = javaEnv->CallStaticObjectMethod(cls, mid, fileString);

    javaEnv->DeleteLocalRef(cls);
    javaEnv->DeleteLocalRef(fileString);

    return result;
}
