//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef PXR_IMAGING_HD_ST_RENDER_PASS_STATE_H
#define PXR_IMAGING_HD_ST_RENDER_PASS_STATE_H

#include "pxr/pxr.h"
#include "pxr/imaging/hdSt/api.h"
#include "pxr/imaging/hd/renderPassState.h"
#include "pxr/imaging/hgi/graphicsCmdsDesc.h"

#include <memory>

PXR_NAMESPACE_OPEN_SCOPE

struct HgiGraphicsPipelineDesc;
struct HgiDepthStencilState;
struct HgiMultiSampleState;
struct HgiRasterizationState;
class HgiCapabilities;

using HdResourceRegistrySharedPtr = std::shared_ptr<class HdResourceRegistry>;
using HdStRenderPassStateSharedPtr = std::shared_ptr<class HdStRenderPassState>;

using HdBufferArrayRangeSharedPtr = std::shared_ptr<class HdBufferArrayRange>;

using HdStShaderCodeSharedPtr = std::shared_ptr<class HdStShaderCode>;
using HdStLightingShaderSharedPtr = std::shared_ptr<class HdStLightingShader>;
using HdStRenderPassShaderSharedPtr =
    std::shared_ptr<class HdStRenderPassShader>;
using HdSt_FallbackLightingShaderSharedPtr =
    std::shared_ptr<class HdSt_FallbackLightingShader>;
using HdSt_GeometricShaderSharedPtr =
    std::shared_ptr<class HdSt_GeometricShader>;
using HdStShaderCodeSharedPtrVector = std::vector<HdStShaderCodeSharedPtr>;
class HdRenderIndex;
class HdSt_ResourceBinder;

/// \class HdStRenderPassState
///
/// A set of rendering parameters used among render passes.
///
/// Parameters are expressed as GL states, uniforms or shaders.
///
class HdStRenderPassState : public HdRenderPassState
{
public:
    HDST_API
    HdStRenderPassState();
    HDST_API
    HdStRenderPassState(HdStRenderPassShaderSharedPtr const &shader);
    HDST_API
    ~HdStRenderPassState() override;

    HDST_API
    void
    Prepare(HdResourceRegistrySharedPtr const &resourceRegistry) override;

    /// XXX: Bind and Unbind set./restore the following GL state.
    /// This will be reworked to use Hgi in the near future.
    /// Following states may be changed and restored to
    /// the GL default at Unbind().
    ///   glEnable(GL_BLEND);
    ///   glEnable(GL_CULL_FACE);
    ///   glEnable(GL_POLYGON_OFFSET_FILL)
    ///   glEnable(GL_PROGRAM_POINT_SIZE);
    ///   glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE)
    ///   glEnable(GL_DEPTH_TEST);
    ///   glEnable(GL_STENCIL_TEST);
    ///   glPolygonOffset()
    ///   glBlend*()
    ///   glColorMask()
    ///   glCullFace()
    ///   glDepthFunc()
    ///   glDepthMask()
    ///   glLineWidth()
    ///   glStencilFunc()
    ///   glStencilOp()
    HDST_API
    void Bind(HgiCapabilities const &hgiCapabilities);
    HDST_API
    void Unbind(HgiCapabilities const &hgiCapabilities);

    /// If set to true (default) and the render pass is rendering into a
    /// multi-sampled aovs, the aovs will be resolved at the end of the render
    /// pass. If false or the aov is not multi-sampled or the render pass is not
    /// rendering into the multi-sampled aov, no resolution takes place.
    HD_API
    void SetResolveAovMultiSample(bool state);
    HD_API
    bool GetResolveAovMultiSample() const;

    /// Set lighting shader
    HDST_API
    void SetLightingShader(HdStLightingShaderSharedPtr const &lightingShader);
    HdStLightingShaderSharedPtr const & GetLightingShader() const {
        return _lightingShader;
    }

    /// renderpass shader
    HDST_API
    void SetRenderPassShader(HdStRenderPassShaderSharedPtr const &renderPassShader);
    HdStRenderPassShaderSharedPtr const &GetRenderPassShader() const {
        return _renderPassShader;
    }

    HDST_API
    void ApplyStateFromGeometricShader(
        HdSt_ResourceBinder const &binder,
        HdSt_GeometricShaderSharedPtr const &geometricShader);

    HDST_API
    void ApplyStateFromCamera();

    /// scene materials
    HDST_API
    void SetUseSceneMaterials(bool state);
    bool GetUseSceneMaterials() const {
        return _useSceneMaterials;
    }

    /// returns shaders (lighting/renderpass)
    HDST_API
    HdStShaderCodeSharedPtrVector GetShaders() const;

    HDST_API
    size_t GetShaderHash() const;

    /// Camera setter API
    ///
    /// Set matrices, viewport and clipping planes explicitly that are used
    /// when there is no HdCamera in the render pass state.
    ///
    /// This is used by render pass that do not have an associated HdCamera
    /// such as the shadow render pass.
    HDST_API
    void SetCameraFramingState(GfMatrix4d const &worldToViewMatrix,
                               GfMatrix4d const &projectionMatrix,
                               GfVec4d const &viewport,
                               ClipPlanesVector const & clipPlanes);
    
    GfMatrix4d GetCullMatrix() const { return _cullMatrix; }

    /// Overrides the case when no HdCamera is given. In the case, uses
    /// matrix specified by SetCameraFramingState.
    HDST_API
    GfMatrix4d GetWorldToViewMatrix() const override;

    /// Overrides the case when no HdCamera is given. In the case, uses
    /// matrix specified by SetCameraFramingState.
    HDST_API
    GfMatrix4d GetProjectionMatrix() const override;

    /// Overrides the case when no HdCamera is given. In the case, uses
    /// clip planes specified by SetCameraFramingState.
    HDST_API ClipPlanesVector const & GetClipPlanes() const override;

    // Helper to get graphics cmds descriptor describing textures
    // we render into and the blend state, constructed from
    // AOV bindings.
    //
    HDST_API
    HgiGraphicsCmdsDesc MakeGraphicsCmdsDesc(
                HdRenderIndex const * renderIndex) const;

    // Helper to initialize graphics pipeline descriptor state including
    // any additional state from the geometric shader.
    HDST_API
    void InitGraphicsPipelineDesc(
                HgiGraphicsPipelineDesc * pipeDesc,
                HdSt_GeometricShaderSharedPtr const & geometricShader) const;

    /// Generates the hash for the settings used to init the graphics pipeline.
    HDST_API
    uint64_t GetGraphicsPipelineHash() const;

private:
    bool _UseAlphaMask() const;

    // Helper to set up the aov attachment desc so that it matches the blend
    // setting of the render pipeline state.
    // If an aovIndex is specified then the color mask will be correlated.
    void _InitAttachmentDesc(HgiAttachmentDesc &attachmentDesc,
                             int aovIndex = -1) const;

    void _InitPrimitiveState(
                HgiGraphicsPipelineDesc * pipeDesc,
                HdSt_GeometricShaderSharedPtr const & geometricShader) const;
    void _InitAttachmentState(HgiGraphicsPipelineDesc * pipeDesc) const;
    void _InitDepthStencilState(HgiDepthStencilState * depthState) const;
    void _InitMultiSampleState(HgiMultiSampleState * multisampleState) const;
    void _InitRasterizationState(
                HgiRasterizationState * rasterizationState,
                HdSt_GeometricShaderSharedPtr const & geometricShader) const;

    // ---------------------------------------------------------------------- //
    // Camera state used when no HdCamera available
    // ---------------------------------------------------------------------- //
    
    GfMatrix4d _worldToViewMatrix;
    GfMatrix4d _projectionMatrix;
    ClipPlanesVector _clipPlanes;

    GfMatrix4d _cullMatrix; // updated during Prepare(..)

    // ---------------------------------------------------------------------- //
    // Shader Objects
    // ---------------------------------------------------------------------- //
    HdStRenderPassShaderSharedPtr _renderPassShader;
    HdSt_FallbackLightingShaderSharedPtr _fallbackLightingShader;
    HdStLightingShaderSharedPtr _lightingShader;

    HdBufferArrayRangeSharedPtr _renderPassStateBar;
    size_t _clipPlanesBufferSize;
    float _alphaThresholdCurrent;
    bool _resolveMultiSampleAov;
    bool _useSceneMaterials;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif  // PXR_IMAGING_HD_ST_RENDER_PASS_STATE_H
