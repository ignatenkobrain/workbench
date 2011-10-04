#ifndef __ABSTRACT_ALGORITHM_H__
#define __ABSTRACT_ALGORITHM_H__

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

//make it easy to use the progress object, don't just forward declare
#include "ProgressObject.h"
#include "CaretAssert.h"

namespace caret {

    class AlgorithmParameters;

    ///the constructor for algorithms does the processing, because constructor/execute cycles don't make sense for something this simple
    class AbstractAlgorithm
    {
        ProgressObject* m_progObj;//so that the destructor can make sure the bar finishes
        bool m_finish;
        AbstractAlgorithm();//prevent default construction
    protected:
        ///override this with the weights of the algorithms this algorithm will call
        static float getSubAlgorithmWeight();//protected so that people don't try to use them to set algorithm weights in progress objects
        ///override this with the amount of work the algorithm does internally, outside of calls to other algorithms
        static float getAlgorithmInternalWeight();
        AbstractAlgorithm(ProgressObject* myProgressObject);
        virtual ~AbstractAlgorithm();
    public:
        ///use this to set the weight parameter of a ProgressObject
        static float getAlgorithmWeight();

        ///override these to allow algorithm parsers to use your algorithm without writing an explicit command class
        static AlgorithmParameters* getParameters() { CaretAssert(false); return NULL; };

        ///override these to allow algorithm parsers to use your algorithm without writing an explicit command class
        static void useParameters(AlgorithmParameters*, ProgressObject*) { CaretAssert(false); };
    };

    ///interface class for algorithm parsers
    struct AutoAlgorithmInterface
    {
        virtual AlgorithmParameters* getParameters() = 0;
        virtual void useParameters(AlgorithmParameters* a, ProgressObject* b) = 0;
    };

    ///templated interface class to pass through to something that inherits from AbstractAlgorithm (or implements equivalent functions)
    template<typename T>
    struct TemplateAutoAlgorithm : public AutoAlgorithmInterface
    {
        AString m_switch, m_shortInfo;
        TemplateAutoAlgorithm(const AString& switchIn, const AString& shortInfo) : m_switch(switchIn), m_shortInfo(shortInfo) { };
        AlgorithmParameters* getParameters() { return T::getParameters(); };
        void useParameters(AlgorithmParameters* a, ProgressObject* b) { T::useParemeters(a, b); };
    };

    ///templated interface class for parsers to inherit from
    class AlgorithmParserInterface
    {
        AutoAlgorithmInterface* m_dummy;
        AlgorithmParserInterface();//must take an interface object, for its vtable to the real algorithm
    public:
        AlgorithmParserInterface(AutoAlgorithmInterface* dummy) : m_dummy(dummy) { };
        virtual ~AlgorithmParserInterface();
    };
}
#endif //__ABSTRACT_ALGORITHM_H__
