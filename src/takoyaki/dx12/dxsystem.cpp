// Copyright(c) 2015 kittikun
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sub license, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "pch.h"
#include "dxsystem.h"

#include "device.h"

namespace Takoyaki
{
    void MemcpySubresource(D3D12_MEMCPY_DEST* dst, D3D12_SUBRESOURCE_DATA* src, uint_fast32_t numRows, uint_fast64_t rowSizeByte, uint_fast32_t numSlices)
    {
        // Row-by-row copy
        for (uint_fast32_t z = 0; z < numSlices; ++z) {
            uint8_t* dstSlice = reinterpret_cast<uint8_t*>(dst->pData) + dst->SlicePitch * z;
            const uint8_t* srcSlice = reinterpret_cast<const uint8_t*>(src->pData) + src->SlicePitch * z;

            for (uint_fast32_t y = 0; y < numRows; ++y) {
                memcpy(dstSlice + dst->RowPitch * y, srcSlice + src->RowPitch * y, rowSizeByte);
            }
        }
    }

    uint_fast64_t UpdateSubresources(const UpdateSubresourceDesc& params, uint_fast64_t requiredSize, const D3D12_PLACED_SUBRESOURCE_FOOTPRINT* layouts, const uint_fast32_t* numRows, const uint_fast64_t* rowSizesInBytes)
    {
        D3D12_RESOURCE_DESC intermediateDesc = params.intermediate->GetDesc();
        D3D12_RESOURCE_DESC destinationDesc = params.destinationResource->GetDesc();

        // some quick checks
        if (intermediateDesc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER || intermediateDesc.Width < requiredSize + layouts[0].Offset)
            throw new std::runtime_error{ "UpdateSubresources, intermediate description invalid" };

        if (destinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER && (params.firstSubResource != 0 || params.numSubResource != 1))
            throw new std::runtime_error{ "UpdateSubresources, destination description invalid" };

        uint8_t* data;
        HRESULT hr = params.intermediate->Map(0, NULL, reinterpret_cast<void**>(&data));

        if (FAILED(hr))
            throw new std::runtime_error{ "Failed to map intermediate resource" };

        for (uint_fast32_t i = 0; i < params.numSubResource; ++i) {
            D3D12_MEMCPY_DEST dstData;
            
            dstData.pData = data + layouts[i].Offset;
            dstData.RowPitch = layouts[i].Footprint.RowPitch;
            dstData.SlicePitch = layouts[i].Footprint.RowPitch * numRows[i];
            
            MemcpySubresource(&dstData, &params.srcData[i], numRows[i], rowSizesInBytes[i], layouts[i].Footprint.Depth);
        }

        params.intermediate->Unmap(0, NULL);

        if (destinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER) {
            params.cmdList->CopyBufferRegion(params.destinationResource, 0, params.intermediate, layouts[0].Offset, layouts[0].Footprint.Width);
        } else {
            // D3D12_RESOURCE_DIMENSION_TEXTURE*
            for (uint_fast32_t i = 0; i < params.numSubResource; ++i) {
                D3D12_TEXTURE_COPY_LOCATION dst;

                dst.pResource = params.destinationResource;
                dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
                dst.SubresourceIndex = i + params.firstSubResource;

                D3D12_TEXTURE_COPY_LOCATION src;

                src.pResource = params.intermediate;
                src.PlacedFootprint = layouts[i];

                params.cmdList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
            }
        }
        return requiredSize;
    }

    uint_fast64_t UpdateSubresourcesHeapAlloc(const UpdateSubresourceDesc& params)
    {
        // check parameters
        if (params.firstSubResource > D3D12_REQ_SUBRESOURCES)
            throw new std::runtime_error{ "UpdateSubresourcesHeapAlloc firstSubResource too big" };

        if (params.numSubResource > (D3D12_REQ_SUBRESOURCES - params.firstSubResource))
            throw new std::runtime_error{ "UpdateSubresourcesHeapAlloc numSubResource too big" };

        uint_fast64_t requiredSize = 0;
        uint_fast64_t memToAlloc = static_cast<uint_fast64_t>(sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(uint_fast32_t) + sizeof(uint_fast64_t)) * params.numSubResource;

        if (memToAlloc > UINTPTR_MAX) {
            throw new std::runtime_error{ "UpdateSubresourcesHeapAlloc memToAlloc too big" };
        }

        std::vector<uint8_t> mem;

        mem.resize(memToAlloc);

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT* layout = reinterpret_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(&mem.front());
        uint_fast64_t* rowSizesInBytes = reinterpret_cast<uint_fast64_t*>(layout + params.numSubResource);
        UINT* numRows = reinterpret_cast<uint_fast32_t*>(rowSizesInBytes + params.numSubResource);

        D3D12_RESOURCE_DESC desc = params.destinationResource->GetDesc();
        auto device = params.device.lock();

        // the following is not thread safe so lock
        {
            auto lock = device->getDeviceLock();

            device->getDXDevice()->GetCopyableFootprints(&desc, params.firstSubResource, params.numSubResource, params.intermediateOffset, layout, numRows, rowSizesInBytes, &requiredSize);
        }

        return UpdateSubresources(params, requiredSize, layout, numRows, rowSizesInBytes);
    }
} // namespace Takoyaki