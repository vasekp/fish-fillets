#include "afont.h"
#include "ainstance.h"
#include "./files.h"
#include <android/bitmap.h>

AndroidFont::AndroidFont(Instance& instance, const std::string& filename) :
        m_instance(instance),
        m_fontSize(), m_outline()
{
    m_filename = dynamic_cast<SystemFile&>(*instance.files().system(filename)).path();
}

void AndroidFont::setSizes(float fontSize, float outline) {
    m_fontSize = fontSize;
    m_outline = outline;
}

std::vector<std::string> AndroidFont::breakLines(const std::string& text, float width) {
    auto& jni = dynamic_cast<AndroidInstance&>(m_instance).jni;
    auto jFilename = jni->NewStringUTF(m_filename.c_str());
    auto jText = jni->NewStringUTF(text.c_str());
    auto jArray = (jobjectArray)jni->CallObjectMethod(jni.object(), jni.method("breakLines"),
            jText, jFilename, m_fontSize, (int)width);
    auto length = jni->GetArrayLength(jArray);
    std::vector <std::string> ret{};
    ret.reserve(length);
    for(auto i = 0u ; i < length ; i++) {
        auto jLine = (jstring)jni->GetObjectArrayElement(jArray, (int)i);
        auto chars = jni->GetStringUTFChars(jLine, nullptr);
        ret.emplace_back(chars);
        jni->ReleaseStringUTFChars(jLine, chars);
        jni->DeleteLocalRef(jLine);
    }
    jni->DeleteLocalRef(jFilename);
    jni->DeleteLocalRef(jText);
    jni->DeleteLocalRef(jArray);
    return ret;
}

ogl::Texture AndroidFont::renderText(const std::string& text) const {
    auto& jni = dynamic_cast<AndroidInstance&>(m_instance).jni;
    auto jFilename = jni->NewStringUTF(m_filename.c_str());
    auto jText = jni->NewStringUTF(text.c_str());
    auto jBitmap = jni->CallObjectMethod(jni.object(), jni.method("renderText"), jText, jFilename, m_fontSize, m_outline);
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(jni, jBitmap, &info);
    std::uint32_t width = info.width;
    std::uint32_t height = info.height;
    std::size_t stride = info.stride;
    void *pixels;
    AndroidBitmap_lockPixels(jni, jBitmap, &pixels);
    if (!jBitmap)
        Log::fatal("bitmap data null (renderText)");
    auto ret = ogl::Texture::fromImageData(m_instance.graphics().system().ref(), width, height, stride, pixels);
    AndroidBitmap_unlockPixels(jni, jBitmap);
    jni->DeleteLocalRef(jBitmap);
    jni->DeleteLocalRef(jFilename);
    jni->DeleteLocalRef(jText);
    return ret;
}
