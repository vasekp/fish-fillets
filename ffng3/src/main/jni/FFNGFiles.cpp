#include "jnix.h"
#include "FFNGFiles.h"
#include <android/log.h>

bool FFNGFiles::exists(const std::string &path)
{
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGFiles");
	jmethodID mid = javaEnv->GetStaticMethodID(cls, "exists", "(Ljava/lang/String;I)Z");

	jstring pathString = javaEnv->NewStringUTF(path.c_str());

    bool result = javaEnv->CallStaticBooleanMethod(cls, mid, pathString, 1);

    javaEnv->DeleteLocalRef(cls);
    javaEnv->DeleteLocalRef(pathString);

    return result;
}

std::string FFNGFiles::read(const std::string &path) {
	JNIEnv* javaEnv = JNI::getInstance()->getJavaEnv();
	jclass cls = javaEnv->FindClass("cz/ger/ffng/FFNGFiles");
	jmethodID mid = javaEnv->GetStaticMethodID(cls, "read", "(Ljava/lang/String;I)Ljava/lang/String;");

	jstring pathString = javaEnv->NewStringUTF(path.c_str());

	jstring resultString = (jstring)javaEnv->CallStaticObjectMethod(cls, mid, pathString, 1);

	jboolean isCopy;
	const char *cstr = javaEnv->GetStringUTFChars(resultString,	&isCopy);
    std::string result(cstr);

    javaEnv->DeleteLocalRef(cls);
    javaEnv->DeleteLocalRef(pathString);
    javaEnv->DeleteLocalRef(resultString);
    if (isCopy) delete cstr;

    return result;
}