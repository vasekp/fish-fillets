/*
 * jni.cpp
 *
 *  Created on: 5.5.2010
 *      Author: dan
 */

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <android/log.h>
#include <signal.h>
#include "jnix.h"
#include "Log.h"
#include "Application.h"
#include "HelpException.h"
#include "BaseException.h"
#include "FFNGSurface.h"

extern "C"
JNIEXPORT jint JNICALL Java_cz_ger_ffng_FFNGApp_ffngmain(JNIEnv * env, jobject obj)
{
	__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "begin");
	JNI::getInstance()->setJavaContext(env, obj);

    try {
        Application app;

		FFNGSurface::initEGL();
		FFNGSurface::initShaders();

        try {
            app.init(0, NULL);
            app.run();
        }
        catch (HelpException &e) {
            printf("%s\n", e.what());
        }
        catch (BaseException &e) {
            LOG_ERROR(e.info());
        }
        app.shutdown();
    	__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "OK");
        return 0;
    }
    catch (BaseException &e) {
        LOG_ERROR(e.info());
    }
    catch (std::exception &e) {
        LOG_ERROR(ExInfo("std::exception")
                .addInfo("what", e.what()));
    }
    catch (...) {
        LOG_ERROR(ExInfo("unknown exception"));
    }

	__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "FAIL");
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

void JNI::setJavaContext(JNIEnv * env, jobject obj) {
	 m_javaEnv = env;
	 m_javaObj = obj;
	 m_javaCls = m_javaEnv->GetObjectClass(m_javaObj);
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