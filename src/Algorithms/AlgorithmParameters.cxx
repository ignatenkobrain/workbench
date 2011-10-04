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

#include "AlgorithmParameters.h"
#include "CaretAssert.h"

#include "CiftiFile.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"

using namespace std;
using namespace caret;

ParameterComponent::ParameterComponent()
{
}

ParameterComponent::~ParameterComponent()
{
    uint32_t i;
    for (i = 0; i < m_paramList.size(); ++i)
    {
        delete m_paramList[i];
    }
    for (i = 0; i < m_optionList.size(); ++i)
    {
        delete m_optionList[i];
    }
}

AlgorithmParameters::AlgorithmParameters()
{
}

AlgorithmParameters::~AlgorithmParameters()
{
    uint32_t i;
    for (i = 0; i < m_outputList.size(); ++i)
    {
        delete m_outputList[i];
    }
}

void ParameterComponent::addOptionalParameter(OptionalParameter* param)
{
    m_optionList.push_back(param);
}

AbstractParameter* ParameterComponent::getInputParameter(const int32_t key, const AbstractParameter::parameterType type)
{
    uint32_t i;
    for (i = 0; i < m_paramList.size(); ++i)
    {
        if (m_paramList[i]->m_key == key && type == m_paramList[i]->getType())
        {
            return m_paramList[i];
        }
    }
    CaretAssertMessage(false, "Algorithm asked for parameter it didn't specify, or of wrong type");
    return NULL;
}

OptionalParameter* ParameterComponent::getOptionalParameter(const int32_t key)
{
    uint32_t i;
    for (i = 0; i < m_optionList.size(); ++i)
    {
        if (m_optionList[i]->m_key == key)
        {
            return m_optionList[i];
        }
    }
    CaretAssertMessage(false, "Algorithm asked for option it didn't specify");
    return NULL;
}

AbstractParameter* AlgorithmParameters::getOutputParameter(const int32_t key, const AbstractParameter::parameterType type)
{
    uint32_t i;
    for (i = 0; i < m_outputList.size(); ++i)
    {
        if (m_outputList[i]->m_key == key && type == m_outputList[i]->getType())
        {
            return m_outputList[i];
        }
    }
    CaretAssertMessage(false, "Algorithm asked for output it didn't specify, or of wrong type");
    return NULL;
}

//sadly, lots of boilerplate for convenience functions
void ParameterComponent::addCiftiParameter(const int32_t key, const AString& name, const AString& description)
{
    m_paramList.push_back(new CiftiParameter(key, name, description));
}

void ParameterComponent::addDoubleParameter(const int32_t key, const AString& name, const AString& description)
{
    m_paramList.push_back(new DoubleParameter(key, name, description));
}

void ParameterComponent::addFuncParameter(const int32_t key, const AString& name, const AString& description)
{
    m_paramList.push_back(new MetricParameter(key, name, description));
}

void ParameterComponent::addIntParameter(const int32_t key, const AString& name, const AString& description)
{
    m_paramList.push_back(new IntParameter(key, name, description));
}

void ParameterComponent::addLabelParameter(const int32_t key, const AString& name, const AString& description)
{
    m_paramList.push_back(new LabelParameter(key, name, description));
}

void ParameterComponent::addStringParameter(const int32_t key, const AString& name, const AString& description)
{
    m_paramList.push_back(new StringParameter(key, name, description));
}

void ParameterComponent::addSurfaceParameter(const int32_t key, const AString& name, const AString& description)
{
    m_paramList.push_back(new SurfaceParameter(key, name, description));
}

void ParameterComponent::addVolumeParameter(const int32_t key, const AString& name, const AString& description)
{
    m_paramList.push_back(new VolumeParameter(key, name, description));
}

void AlgorithmParameters::setHelpText(const AString& textIn)
{
    m_helpText = textIn;
}

void AlgorithmParameters::addCiftiOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    CiftiParameter* myParam = new CiftiParameter(key, name, description);
    myParam->m_parameter = new CiftiFile();
    m_outputList.push_back(myParam);
}

void AlgorithmParameters::addDoubleOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    m_outputList.push_back(new DoubleParameter(key, name, description));
}

void AlgorithmParameters::addFuncOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    MetricParameter* myParam = new MetricParameter(key, name, description);
    myParam->m_parameter = new MetricFile();
    m_outputList.push_back(myParam);
}

void AlgorithmParameters::addIntOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    m_outputList.push_back(new IntParameter(key, name, description));
}

void AlgorithmParameters::addLabelOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    LabelParameter* myParam = new LabelParameter(key, name, description);
    myParam->m_parameter = new LabelFile();
    m_outputList.push_back(myParam);
}

void AlgorithmParameters::addStringOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    m_outputList.push_back(new StringParameter(key, name, description));
}

void AlgorithmParameters::addSurfaceOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    SurfaceParameter* myParam = new SurfaceParameter(key, name, description);
    myParam->m_parameter = new SurfaceFile();
    m_outputList.push_back(myParam);
}

void AlgorithmParameters::addVolumeOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    VolumeParameter* myParam = new VolumeParameter(key, name, description);
    myParam->m_parameter = new VolumeFile();
    m_outputList.push_back(myParam);
}

const AString& AlgorithmParameters::getHelpText()
{
    return m_helpText;
}

AbstractParameter::~AbstractParameter()
{
}
