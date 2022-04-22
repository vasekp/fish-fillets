#include "jnix.h"
#include "FFNGFiles.h"
#include <android/log.h>

bool FFNGFiles::exists(const std::string &path, int type)
{
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGFiles");
	jmethodID mid = javaEnv->GetStaticMethodID(cls, "exists", "(Ljava/lang/String;I)Z");

	jstring pathString = javaEnv->NewStringUTF(path.c_str());

    bool result = javaEnv->CallStaticBooleanMethod(cls, mid, pathString, type);

    javaEnv->DeleteLocalRef(cls);
    javaEnv->DeleteLocalRef(pathString);

    return result;
}

std::string FFNGFiles::read(const std::string &path, int type) {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGFiles");
	jmethodID mid = javaEnv->GetStaticMethodID(cls, "read", "(Ljava/lang/String;I)Ljava/lang/String;");

	jstring pathString = javaEnv->NewStringUTF(path.c_str());

	jstring resultString = (jstring)javaEnv->CallStaticObjectMethod(cls, mid, pathString, type);

	jboolean isCopy;
	const char *cstr = javaEnv->GetStringUTFChars(resultString,	&isCopy);
    std::string result(cstr);

    javaEnv->DeleteLocalRef(cls);
    javaEnv->DeleteLocalRef(pathString);
    javaEnv->DeleteLocalRef(resultString);
    if (isCopy) delete cstr;

    return result;
}

void FFNGFiles::createPath(const std::string &path) {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGFiles");
	jmethodID mid = javaEnv->GetStaticMethodID(cls, "createPath", "(Ljava/lang/String;)V");

	jstring pathString = javaEnv->NewStringUTF(path.c_str());

	javaEnv->CallStaticVoidMethod(cls, mid, pathString);

    javaEnv->DeleteLocalRef(cls);
	javaEnv->DeleteLocalRef(pathString);
}

bool FFNGFiles::write(const std::string &path, const std::string &data) {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGFiles");
	jmethodID mid = javaEnv->GetStaticMethodID(cls, "write", "(Ljava/lang/String;Ljava/lang/String;)Z");

	jstring pathString = javaEnv->NewStringUTF(path.c_str());
	jstring dataString = javaEnv->NewStringUTF(data.c_str());

	bool result = javaEnv->CallStaticBooleanMethod(cls, mid, pathString, dataString);

    javaEnv->DeleteLocalRef(cls);
	javaEnv->DeleteLocalRef(pathString);
	javaEnv->DeleteLocalRef(dataString);

	return result;
}
