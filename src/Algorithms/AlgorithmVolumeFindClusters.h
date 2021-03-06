#ifndef __ALGORITHM_VOLUME_FIND_CLUSTERS_H__
#define __ALGORITHM_VOLUME_FIND_CLUSTERS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include "AbstractAlgorithm.h"

namespace caret {
    
    class AlgorithmVolumeFindClusters : public AbstractAlgorithm
    {
        AlgorithmVolumeFindClusters();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmVolumeFindClusters(ProgressObject* myProgObj, const VolumeFile* volIn, const float& threshValue, const float& minVolume,
                                    VolumeFile* volOut, const bool& lessThan = false, const VolumeFile* myRoi = NULL, const int& subvolNum = -1,
                                    const int& startVal = 1, int* endVal = NULL, const float& sizeRatio = -1.0f, const float& distanceCutoff = -1.0f);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmVolumeFindClusters> AutoAlgorithmVolumeFindClusters;

}

#endif //__ALGORITHM_VOLUME_FIND_CLUSTERS_H__
