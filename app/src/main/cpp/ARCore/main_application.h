//
// Created by sgodi on 8/31/2021.
//

#ifndef DIMENSION_MAIN_APPLICATION_H
#define DIMENSION_MAIN_APPLICATION_H

#define  LOG_TAG    "dimension-debugger"

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/asset_manager.h>
#include <android/log.h>
#include <jni.h>

#include <memory>
#include <set>
#include <string>
#include <unordered_map>

#include "arcore_c_api.h"
#include "glm.h"
#include "background_renderer.h"
#include "object_renderer.h"
#include "plane_renderer.h"
#include "point_cloud_renderer.h"
#include "texture.h"
#include "util.h"

namespace arcore
{

class MainApplication {

public:
    // Constructor and deconstructor.
    explicit MainApplication(AAssetManager* asset_manager);
    ~MainApplication();

    // OnPause is called on the UI thread from the Activity's onPause method.
    void OnPause();

    // OnResume is called on the UI thread from the Activity's onResume method.
    void OnResume(JNIEnv* env, void* context, void* activity);

    // OnSurfaceCreated is called on the OpenGL thread when GLSurfaceView
    // is created.
    void OnSurfaceCreated();

    // OnDisplayGeometryChanged is called on the OpenGL thread when the
    // render surface size or display rotation changes.
    //
    // @param display_rotation: current display rotation.
    // @param width: width of the changed surface view.
    // @param height: height of the changed surface view.
    void OnDisplayGeometryChanged(int display_rotation, int width, int height);

    // OnDrawFrame is called on the OpenGL thread to render the next frame.
    void OnDrawFrame(bool depthColorVisualizationEnabled,
                     bool useDepthForOcclusion);

    // OnTouched is called on the OpenGL thread after the user touches the screen.
    // @param x: x position on the screen (pixels).
    // @param y: y position on the screen (pixels).
    void OnTouched(float x, float y);

    // Returns true if any planes have been detected.  Used for hiding the
    // "searching for planes" snackbar.
    bool HasDetectedPlanes() const { return plane_count_ > 0; }

    // Returns true if depth is supported.
    bool IsDepthSupported();

    void OnSettingsChange(bool is_instant_placement_enabled);

    void MoveObject(float x, float y);

    void RotateObject(float x, float y);

    void ScaleObject(float scale);

private:
    glm::mat3 GetTextureTransformMatrix(const ArSession* session,
                                        const ArFrame* frame);
    ArSession* ar_session_ = nullptr;
    ArFrame* ar_frame_ = nullptr;

    bool install_requested_ = false;
    bool calculate_uv_transform_ = false;
    int width_ = 1;
    int height_ = 1;
    int display_rotation_ = 0;
    bool is_instant_placement_enabled_ = true;

    AAssetManager* const asset_manager_;

    // The anchors at which we are drawing android models using given colors.
    struct ColoredAnchor {
        ArAnchor* anchor;
        ArTrackable* trackable;
        float color[4];
    };

    std::vector<ColoredAnchor> anchors_;

    PointCloudRenderer point_cloud_renderer_;
    BackgroundRenderer background_renderer_;
    PlaneRenderer plane_renderer_;
    ObjRenderer andy_renderer_;
    Texture depth_texture_;

    float scaleFactor_ = 1.0f;
    float verticalRotation_ = 0.0f;
    float horizontalRotation_ = 0.0f;
    glm::vec3 verticalTranslate_ = glm::vec3(0);
    glm::vec3 horizontalTranslate_ = glm::vec3(0);
    glm::mat4 view_matrix_ = glm::mat4(1);

    int32_t plane_count_ = 0;

    void ConfigureSession();

    void UpdateAnchorColor(ColoredAnchor* colored_anchor);
};
}




#endif //DIMENSION_MAIN_APPLICATION_H
