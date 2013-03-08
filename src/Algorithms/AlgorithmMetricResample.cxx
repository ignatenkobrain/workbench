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

#include "AlgorithmMetricResample.h"
#include "AlgorithmException.h"

#include "CaretLogger.h"
#include "MetricFile.h"
#include "PaletteColorMapping.h"
#include "SurfaceFile.h"
#include "SurfaceResamplingHelper.h"

using namespace caret;
using namespace std;

AString AlgorithmMetricResample::getCommandSwitch()
{
    return "-metric-resample";
}

AString AlgorithmMetricResample::getShortDescription()
{
    return "RESAMPLE A METRIC FILE TO A DIFFERENT MESH";
}

OperationParameters* AlgorithmMetricResample::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addMetricParameter(1, "metric-in", "the metric file to resample");
    
    ret->addSurfaceParameter(2, "current-sphere", "a sphere surface with the mesh that the metric is currently on");
    
    ret->addSurfaceParameter(3, "new-sphere", "a sphere surface that is in register with <current-sphere> and has the desired output mesh");
    
    ret->addStringParameter(4, "method", "the method name");
    
    ret->addMetricOutputParameter(5, "metric-out", "the output metric");
    
    OptionalParameter* areaSurfsOpt = ret->createOptionalParameter(6, "-area-surfs", "specify surfaces to do vertex area correction based on");
    areaSurfsOpt->addSurfaceParameter(1, "current-area", "a relevant anatomical surface with <current-sphere> mesh");
    areaSurfsOpt->addSurfaceParameter(2, "new-area", "a relevant anatomical surface with <new-sphere> mesh");
    
    AString myHelpText =
        AString("Resamples a metric file, given two spherical surfaces that are in register.  ") +
        "If -area-surfs are not specified, the sphere surfaces are used for area correction, if the method used does area correction.\n\n" +
        "The <method> argument must be one of the following:\n\n";
    
    vector<SurfaceResamplingMethodEnum::Enum> allEnums;
    SurfaceResamplingMethodEnum::getAllEnums(allEnums);
    for (int i = 0; i < (int)allEnums.size(); ++i)
    {
        myHelpText += SurfaceResamplingMethodEnum::toName(allEnums[i]) + "\n";
    }
    
    myHelpText += "\nThe ADAP_BARY_AREA method is recommended for metric data, because it should use all data while downsampling, unlike BARYCENTRIC.";
    ret->setHelpText(myHelpText);
    return ret;
}

void AlgorithmMetricResample::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    MetricFile* metricIn = myParams->getMetric(1);
    SurfaceFile* curSphere = myParams->getSurface(2);
    SurfaceFile* newSphere = myParams->getSurface(3);
    bool ok = false;
    SurfaceResamplingMethodEnum::Enum myMethod = SurfaceResamplingMethodEnum::fromName(myParams->getString(4), &ok);
    if (!ok)
    {
        throw AlgorithmException("invalid method name");
    }
    MetricFile* metricOut = myParams->getOutputMetric(5);
    SurfaceFile* curArea = curSphere, *newArea = newSphere;
    OptionalParameter* areaSurfsOpt = myParams->getOptionalParameter(6);
    if (areaSurfsOpt->m_present)
    {
        switch(myMethod)
        {
            case SurfaceResamplingMethodEnum::BARYCENTRIC:
                CaretLogInfo("This method does not use area correction, -area-surfs is not needed");
                break;
            default:
                break;
        }
        curArea = areaSurfsOpt->getSurface(1);
        newArea = areaSurfsOpt->getSurface(2);
    }
    AlgorithmMetricResample(myProgObj, metricIn, curSphere, newSphere, myMethod, metricOut, curArea, newArea);
}

AlgorithmMetricResample::AlgorithmMetricResample(ProgressObject* myProgObj, const MetricFile* metricIn, const SurfaceFile* curSphere, const SurfaceFile* newSphere,
                                                 const SurfaceResamplingMethodEnum::Enum& myMethod, MetricFile* metricOut, const SurfaceFile* curArea, const SurfaceFile* newArea) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (metricIn->getNumberOfNodes() != curSphere->getNumberOfNodes()) throw AlgorithmException("input metric has different number of nodes than input sphere");
    switch (myMethod)
    {
        case SurfaceResamplingMethodEnum::BARYCENTRIC:
            break;
        default:
            if (curArea == NULL || newArea == NULL) throw AlgorithmException("specified method does area correction, but no area surfaces given");
            if (curSphere->getNumberOfNodes() != curArea->getNumberOfNodes()) throw AlgorithmException("current area surface has different number of nodes than current sphere");
            if (newSphere->getNumberOfNodes() != newArea->getNumberOfNodes()) throw AlgorithmException("new area surface has different number of nodes than new sphere");
    }
    int numColumns = metricIn->getNumberOfColumns(), numNewNodes = newSphere->getNumberOfNodes();
    metricOut->setNumberOfNodesAndColumns(numNewNodes, numColumns);
    metricOut->setStructure(newSphere->getStructure());
    vector<float> colScratch(numNewNodes, 0.0f);
    SurfaceResamplingHelper myHelp(myMethod, curSphere, newSphere, curArea, newArea);
    for (int i = 0; i < numColumns; ++i)
    {
        metricOut->setColumnName(i, metricIn->getColumnName(i));
        *metricOut->getPaletteColorMapping(i) = *metricIn->getPaletteColorMapping(i);
        myHelp.resampleNormal(metricIn->getValuePointerForColumn(i), colScratch.data());
        metricOut->setValuesForColumn(i, colScratch.data());
    }
}

float AlgorithmMetricResample::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricResample::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}