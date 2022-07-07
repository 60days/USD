//
// Copyright 2020 Pixar
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
#include "pxr/imaging/hgiMetal/capabilities.h"
#include "pxr/imaging/hgiMetal/hgi.h"

#include "pxr/base/arch/defines.h"
#include <Metal/Metal.h>

PXR_NAMESPACE_OPEN_SCOPE

HgiMetalCapabilities::HgiMetalCapabilities(id<MTLDevice> device)
{
    if (@available(macOS 10.14.5, ios 12.0, *)) {
        _SetFlag(HgiDeviceCapabilitiesBitsConcurrentDispatch, true);
    }

    defaultStorageMode = MTLResourceStorageModeShared;
    bool unifiedMemory = false;
    if (@available(macOS 100.100, ios 12.0, *)) {
        unifiedMemory = true;
    } else if (@available(macOS 10.15, ios 13.0, *)) {
#if defined(ARCH_OS_IOS) || (defined(__MAC_10_15) && __MAC_OS_X_VERSION_MAX_ALLOWED >= __MAC_10_15)
        unifiedMemory = [device hasUnifiedMemory];
#else
        unifiedMemory = [device isLowPower];
#endif
    }

    _SetFlag(HgiDeviceCapabilitiesBitsUnifiedMemory, unifiedMemory);

    _SetFlag(HgiDeviceCapabilitiesBitsBuiltinBarycentrics, true);

    _SetFlag(HgiDeviceCapabilitiesBitsShaderDoublePrecision, false);

#if defined(ARCH_OS_MACOS)
    if (!unifiedMemory) {
        defaultStorageMode = MTLResourceStorageModeManaged;
    }
#endif

    _maxUniformBlockSize          = 64 * 1024;
    _maxShaderStorageBlockSize    = 1 * 1024 * 1024 * 1024;
    _uniformBufferOffsetAlignment = 16;
}

HgiMetalCapabilities::~HgiMetalCapabilities() = default;

int
HgiMetalCapabilities::GetAPIVersion() const
{
    if (@available(macOS 10.15, ios 13.0, *)) {
        return APIVersion_Metal3_0;
    }
    if (@available(macOS 10.13, ios 11.0, *)) {
        return APIVersion_Metal2_0;
    }
    
    return APIVersion_Metal1_0;
}

int
HgiMetalCapabilities::GetShaderVersion() const
{
    // Note: This is not the Metal Shader Language version. It is provided for
    // compatibility with code that is asking for the GLSL version.
    return 450;
}

PXR_NAMESPACE_CLOSE_SCOPE
