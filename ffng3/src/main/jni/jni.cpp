/*
 * jni.cpp
 *
 *  Created on: 5.5.2010
 *      Author: dan
 */

#include <filesystem>
#include <android/log.h>
#include <ff/gengine/ResImagePack.h>
#include "jnix.h"
#include "Log.h"
#include "Application.h"
#include "HelpException.h"
#include "BaseException.h"
#include "FFNGSurface.h"

extern "C"
JNIEXPORT jint JNICALL Java_cz_ger_ffng_FFNGApp_ffngMain(JNIEnv * env, jobject obj, jstring jPath, jobject jAssets)
{
	__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "begin");
	JNI::getInstance()->setJavaContext(env, obj, jAssets);

    try {
        Application app;

		FFNGSurface::initEGL();
		FFNGSurface::initShaders();

        {
            const char* storagePath = env->GetStringUTFChars(jPath, nullptr);
            std::filesystem::current_path(storagePath);
            __android_log_print(ANDROID_LOG_DEBUG, "FFNG", "current path %s", storagePath);
            env->ReleaseStringUTFChars(jPath, storagePath);
        }

        try {
            app.init();
            app.run();
        }
        catch (HelpException &e) {
            printf("%s\n", e.what());
        }
        catch (BaseException &e) {
            Log::error("%s", e.info().info().c_str());
        }
        app.shutdown();
        Log::info("Terminating gracefully");
        JNI::getInstance()->clearJavaContext();

        FFNGSurface::termEGL();
        ResImagePack::clearCache();

        return 0;
    }
    catch (BaseException &e) {
        Log::error("%s", e.info().info().c_str());
    }
    catch (std::exception &e) {
        Log::error("%s", e.what());
    }
    catch (...) {
        Log::error("unknown exception");
    }

    Log::info("Terminating after error");
    JNI::getInstance()->clearJavaContext();
    return 1;
}

JNI* JNI::instance = NULL;

JNI::JNI()
: m_javaEnv(NULL)
, m_javaObj(NULL)
, m_javaCls(NULL)
{

}

JNI::~JNI() {
	if (m_javaCls)
		m_javaEnv->DeleteLocalRef(m_javaCls);

	if (instance)
		delete instance;
}

JNI* JNI::getInstance() {
	if (!instance)
		instance = new JNI;
	return instance;
}

void JNI::setJavaContext(JNIEnv * env, jobject obj, jobject jAssets) {
	 m_javaEnv = env;
	 m_javaObj = obj;
	 m_javaCls = env->GetObjectClass(m_javaObj);
     m_assetManager = AAssetManager_fromJava(env, jAssets);
}

void JNI::clearJavaContext() {
    m_javaEnv = nullptr;
    m_javaObj = nullptr;
    m_javaCls = nullptr;
}

int JNI::getInt(jobject obj, const char *fieldName)
{
	jclass cls = m_javaEnv->GetObjectClass(obj);
	jfieldID fid = m_javaEnv->GetFieldID(cls, fieldName, "I");
	m_javaEnv->DeleteLocalRef(cls);
	if (!fid) return 0;

	return m_javaEnv->GetIntField(obj, fid);
}

float JNI::getFloat(jobject obj, const char *fieldName)
{
	jclass cls = m_javaEnv->GetObjectClass(obj);
	jfieldID fid = m_javaEnv->GetFieldID(cls, fieldName, "F");
	m_javaEnv->DeleteLocalRef(cls);
	if (!fid) return 0;

	return m_javaEnv->GetFloatField(obj, fid);
}