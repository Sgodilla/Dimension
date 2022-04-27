//
// Created by sgodi on 8/31/2021.
//

#define _USE_MATH_DEFINES

#include "object_renderer.h"
#include "util.h"
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepTools.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>

#define M_PI           3.14159265358979323846 /* pi */

namespace arcore {
    namespace {
        const glm::vec4 kLightDirection(0.0f, 1.0f, 0.0f, 0.0f);
        constexpr char kVertexShaderFilename[] = "shaders/ar_object.vert";
        constexpr char kFragmentShaderFilename[] = "shaders/ar_object.frag";
        constexpr char kUseDepthForOcclusionShaderFlag[] = "USE_DEPTH_FOR_OCCLUSION";
    }  // namespace

    void ObjRenderer::InitializeGlContent(AAssetManager* asset_manager,
                                          const std::string& obj_file_name,
                                          const std::string& png_file_name) {
        compileAndLoadShaderProgram(asset_manager);
        position_attrib_ = glGetAttribLocation(shader_program_, "a_Position");
        normal_attrib_ = glGetAttribLocation(shader_program_, "a_Normal");
        //tex_coord_attrib_ = glGetAttribLocation(shader_program_, "a_TexCoord");

        //glGenTextures(1, &texture_id_);
        //glBindTexture(GL_TEXTURE_2D, texture_id_);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        /*if (!util::LoadPngFromAssetManager(GL_TEXTURE_2D, png_file_name)) {
            LOGE("Could not load png texture for planes.");
        }*/
        //glGenerateMipmap(GL_TEXTURE_2D);

        //glBindTexture(GL_TEXTURE_2D, 0);

        //util::LoadObjFile(obj_file_name, asset_manager, &vertices_, &normals_, &uvs_, &indices_);

        // Make Bottle
        const double aRadius = 0.25;
        const double aHeight = 0.5;
        BRepPrimAPI_MakeSphere makeSphere(aRadius);
        //TopoDS_Shape aShape = makeSphere.Shape();
        TopoDS_Shape aShape = MakeBottle(aRadius, aHeight, aRadius / 2);

        // Set Mesh Parameters
        IMeshTools_Parameters aMeshParams;
        aMeshParams.Deflection = 0.001;
        aMeshParams.Angle = 0.05;
        aMeshParams.Relative = false;
        aMeshParams.InParallel = true;
        //aMeshParams.MinSize = Precision::Confusion();
        aMeshParams.InternalVerticesMode = true;
        aMeshParams.ControlSurfaceDeflection = true;

        //Mesh Box Bottle
        Mesh(aShape, aMeshParams, vertices_, normals_, uvs_, indices_);

        util::CheckGlError("obj_renderer::InitializeGlContent()");
    }

    void ObjRenderer::setUseDepthForOcclusion(AAssetManager* asset_manager,
                                              bool use_depth_for_occlusion) {
        if (use_depth_for_occlusion_ == use_depth_for_occlusion) {
            return;  // No change, does nothing.
        }

        // Toggles the occlusion rendering mode and recompiles the shader.
        use_depth_for_occlusion_ = use_depth_for_occlusion;
        compileAndLoadShaderProgram(asset_manager);
    }

    void ObjRenderer::compileAndLoadShaderProgram(AAssetManager* asset_manager) {
        // Compiles and loads the shader program based on the selected mode.
        std::map<std::string, int> define_values_map;
        define_values_map[kUseDepthForOcclusionShaderFlag] =
                use_depth_for_occlusion_ ? 1 : 0;

        shader_program_ =
                util::CreateProgram(kVertexShaderFilename, kFragmentShaderFilename,
                                    asset_manager, define_values_map);
        if (!shader_program_) {
            LOGE("Could not create program.");
        }

        mvp_mat_uniform_ =
                glGetUniformLocation(shader_program_, "u_ModelViewProjection");
        mv_mat_uniform_ = glGetUniformLocation(shader_program_, "u_ModelView");
        //texture_uniform_ = glGetUniformLocation(shader_program_, "u_Texture");

        lighting_param_uniform_ =
                glGetUniformLocation(shader_program_, "u_LightingParameters");
        material_param_uniform_ =
                glGetUniformLocation(shader_program_, "u_MaterialParameters");
        color_correction_param_uniform_ =
                glGetUniformLocation(shader_program_, "u_ColorCorrectionParameters");
        color_uniform_ = glGetUniformLocation(shader_program_, "u_ObjColor");

        // Occlusion Uniforms.
        if (use_depth_for_occlusion_) {
            depth_texture_uniform_ =
                    glGetUniformLocation(shader_program_, "u_DepthTexture");
            depth_uv_transform_uniform_ =
                    glGetUniformLocation(shader_program_, "u_DepthUvTransform");
            depth_aspect_ratio_uniform_ =
                    glGetUniformLocation(shader_program_, "u_DepthAspectRatio");
        }
    }

    void ObjRenderer::SetMaterialProperty(float ambient, float diffuse,
                                          float specular, float specular_power) {
        ambient_ = ambient;
        diffuse_ = diffuse;
        specular_ = specular;
        specular_power_ = specular_power;
    }

    void ObjRenderer::Draw(const glm::mat4& projection_mat,
                           const glm::mat4& view_mat, const glm::mat4& model_mat,
                           const float* color_correction4,
                           const float* object_color4) const {
        if (!shader_program_) {
            LOGE("shader_program is null.");
            util::CheckGlError("obj_renderer::Draw(), Load Shader Program");
            return;
        }

        glUseProgram(shader_program_);

        util::CheckGlError("obj_renderer::Draw(), Use Shader Program");

        //glActiveTexture(GL_TEXTURE0);
        //glUniform1i(texture_uniform_, 0);
        //glBindTexture(GL_TEXTURE_2D, texture_id_);

        util::CheckGlError("obj_renderer::Draw(). Bind Textures");

        glm::mat4 mvp_mat = projection_mat * view_mat * model_mat;
        glm::mat4 mv_mat = view_mat * model_mat;
        glm::vec4 view_light_direction = glm::normalize(mv_mat * kLightDirection);

        glUniform4f(lighting_param_uniform_, view_light_direction[0],
                    view_light_direction[1], view_light_direction[2], 1.f);
        glUniform4f(material_param_uniform_, ambient_, diffuse_, specular_,
                    specular_power_);
        glUniform4fv(color_correction_param_uniform_, 1, color_correction4);
        glUniform4fv(color_uniform_, 1, object_color4);

        glUniformMatrix4fv(mvp_mat_uniform_, 1, GL_FALSE, glm::value_ptr(mvp_mat));
        glUniformMatrix4fv(mv_mat_uniform_, 1, GL_FALSE, glm::value_ptr(mv_mat));

        util::CheckGlError("obj_renderer::Draw(), Set Uniforms");

        // Occlusion parameters.
        if (use_depth_for_occlusion_) {
            // Attach the depth texture.
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depth_texture_id_);
            glUniform1i(depth_texture_uniform_, 1);

            // Set the depth texture uv transform.
            glUniformMatrix3fv(depth_uv_transform_uniform_, 1, GL_FALSE,
                               glm::value_ptr(uv_transform_));
            glUniform1f(depth_aspect_ratio_uniform_, depth_aspect_ratio_);
        }

        // Note: for simplicity, we are uploading the model each time we draw it.  A
        // real application should use vertex buffers to upload the geometry once.
        glEnableVertexAttribArray(position_attrib_);
        glVertexAttribPointer(position_attrib_, 3, GL_FLOAT, GL_FALSE, 0,
                              vertices_.data());

        util::CheckGlError("obj_renderer::Draw(), Enable Vertex Position Array");

        glEnableVertexAttribArray(normal_attrib_);
        glVertexAttribPointer(normal_attrib_, 3, GL_FLOAT, GL_FALSE, 0,
                              normals_.data());

        util::CheckGlError("obj_renderer::Draw(), Enable Vertex Normal Array");

        //glEnableVertexAttribArray(tex_coord_attrib_);
        //glVertexAttribPointer(tex_coord_attrib_, 2, GL_FLOAT, GL_FALSE, 0, uvs_.data());

        util::CheckGlError("obj_renderer::Draw(), Enable Vertex Texture Array");

        glDepthMask(GL_TRUE);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        // Textures are loaded with premultiplied alpha
        // (https://developer.android.com/reference/android/graphics/BitmapFactory.Options#inPremultiplied),
        // so we use the premultiplied alpha blend factors.
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_SHORT,
                       indices_.data());
        util::CheckGlError("obj_renderer::Draw(), Draw Function");

        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glDisableVertexAttribArray(position_attrib_);
        glDisableVertexAttribArray(normal_attrib_);
        //glDisableVertexAttribArray(tex_coord_attrib_);

        glUseProgram(0);
        util::CheckGlError("obj_renderer::Draw(), End Program");
    }

}  // namespace arcore