//
// Created by sgodi on 8/31/2021.
//

#ifndef DIMENSION_POINT_CLOUD_RENDERER_H
#define DIMENSION_POINT_CLOUD_RENDERER_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/asset_manager.h>
#include <cstdlib>
#include <vector>
#include "arcore_c_api.h"
#include "glm.h"

namespace arcore {

    class PointCloudRenderer {
    public:
        // Default constructor of PointCloudRenderer.
        PointCloudRenderer() = default;

        // Default deconstructor of PointCloudRenderer.
        ~PointCloudRenderer() = default;

        // Initialize the GL content, needs to be called on GL thread.
        void InitializeGlContent(AAssetManager* asset_manager);

        // Render the AR point cloud.
        //
        // @param mvp_matrix, the model view projection matrix of point cloud.
        // @param ar_session, the session that is used to query point cloud points
        //     from ar_point_cloud.
        // @param ar_point_cloud, point cloud data to for rendering.
        void Draw(const glm::mat4& mvp_matrix, ArSession* ar_session,
                  ArPointCloud* ar_point_cloud) const;

    private:
        GLuint shader_program_;
        GLint attribute_vertices_;
        GLint uniform_mvp_mat_;
        GLint uniform_color_;
        GLint uniform_point_size_;
    };
}  // namespace arcore

#endif //DIMENSION_POINT_CLOUD_RENDERER_H
