#ifndef __SURFACE_RESAMPLING_HELPER_H__
#define __SURFACE_RESAMPLING_HELPER_H__

/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "CaretPointer.h"
#include "SurfaceResamplingMethodEnum.h"

#include <map>
#include <vector>

namespace caret {

    class SurfaceFile;
    
    class SurfaceResamplingHelper
    {
        SurfaceResamplingHelper();
        struct WeightElem
        {
            int node;
            float weight;
            WeightElem() { }
            WeightElem(const int& nodeIn, const float& weightIn) : node(nodeIn), weight(weightIn) { }
        };
        CaretArray<WeightElem> m_storagechunk;
        CaretArray<WeightElem*> m_weights;
        bool checkSphere(const SurfaceFile* surface);
        void changeRadius(const float& radius, const SurfaceFile* input, SurfaceFile* output);
        void computeWeightsAdapBaryArea(const SurfaceFile* currentSphere, const SurfaceFile* newSphere, const SurfaceFile* currentAreaSurf, const SurfaceFile* newAreaSurf);
        void computeWeightsBarycentric(const SurfaceFile* currentSphere, const SurfaceFile* newSphere);
        void makeBarycentricWeights(const SurfaceFile* from, const SurfaceFile* to, std::vector<std::map<int, float> >& weights);
        void compactWeights(const std::vector<std::map<int, float> >& weights);
    public:
        SurfaceResamplingHelper(const SurfaceResamplingMethodEnum::Enum& myMethod, const SurfaceFile* currentSphere, const SurfaceFile* newSphere,
                                const SurfaceFile* currentAreaSurf = NULL, const SurfaceFile* newAreaSurf = NULL);
        ///resample real-valued data by means of weights
        void resampleNormal(const float* input, float* output);
        ///resample 3D coordinate data by means of weights
        void resample3DCoord(const float* input, float* output);
        ///resample label-like data according to which value gets the largest weight sum
        void resamplePopular(const int* input, int* output);
    };

}

#endif //__SURFACE_RESAMPLING_HELPER_H__