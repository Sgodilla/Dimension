//
// Created by sgodi on 8/31/2021.
//

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>

#include "main_application.h"

#define JNI_METHOD(return_type, method_name) \
  JNIEXPORT return_type JNICALL              \
      Java_com_example_dimension_JniInterface_##method_name

extern "C" {

namespace {
// maintain a reference to the JVM so we can use it later.
    static JavaVM *g_vm = nullptr;

    inline jlong jptr(arcore::MainApplication *native_main_application) {
        return reinterpret_cast<intptr_t>(native_main_application);
    }

    inline arcore::MainApplication *native(jlong ptr) {
        return reinterpret_cast<arcore::MainApplication *>(ptr);
    }

}  // namespace

jint JNI_OnLoad(JavaVM *vm, void *) {
    g_vm = vm;
    return JNI_VERSION_1_6;
}

JNI_METHOD(jlong, createNativeApplication)
(JNIEnv *env, jclass, jobject j_asset_manager) {
    AAssetManager *asset_manager = AAssetManager_fromJava(env, j_asset_manager);
    return jptr(new arcore::MainApplication(asset_manager));
}

JNI_METHOD(jboolean, isDepthSupported)
(JNIEnv *, jclass, jlong native_application) {
    return native(native_application)->IsDepthSupported();
}

JNI_METHOD(void, onSettingsChange)
(JNIEnv *, jclass, jlong native_application,
        jboolean is_instant_placement_enabled) {
native(native_application)->OnSettingsChange(is_instant_placement_enabled);
}

JNI_METHOD(void, destroyNativeApplication)
(JNIEnv *, jclass, jlong native_application) {
delete native(native_application);
}

JNI_METHOD(void, onPause)
(JNIEnv *, jclass, jlong native_application) {
native(native_application)->OnPause();
}

JNI_METHOD(void, onResume)
(JNIEnv *env, jclass, jlong native_application, jobject context,
jobject activity) {
native(native_application)->OnResume(env, context, activity);
}

JNI_METHOD(void, onGlSurfaceCreated)
(JNIEnv *, jclass, jlong native_application) {
native(native_application)->OnSurfaceCreated();
}

JNI_METHOD(void, onDisplayGeometryChanged)
(JNIEnv *, jobject, jlong native_application, int display_rotation, int width,
int height) {
native(native_application)
->OnDisplayGeometryChanged(display_rotation, width, height);
}

JNI_METHOD(void, onGlSurfaceDrawFrame)
(JNIEnv *, jclass, jlong native_application,
        jboolean depth_color_visualization_enabled, jboolean use_depth_for_occlusion) {
native(native_application)
->OnDrawFrame(depth_color_visualization_enabled, use_depth_for_occlusion);
}

JNI_METHOD(void, onTouched)
(JNIEnv *, jclass, jlong native_application, jfloat x, jfloat y) {
native(native_application)->OnTouched(x, y);
}

JNI_METHOD(jboolean, hasDetectedPlanes)
(JNIEnv *, jclass, jlong native_application) {
return static_cast<jboolean>(
        native(native_application)->HasDetectedPlanes() ? JNI_TRUE : JNI_FALSE);
}

JNIEnv *GetJniEnv() {
    JNIEnv *env;
    jint result = g_vm->AttachCurrentThread(&env, nullptr);
    return result == JNI_OK ? env : nullptr;
}

jclass FindClass(const char *classname) {
    JNIEnv *env = GetJniEnv();
    return env->FindClass(classname);
}

}  // extern "C"