#ifndef __CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER__H_
#define __CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <set>

#include <QWidget>

#include "EventListenerInterface.h"

class QCheckBox;
class QGridLayout;
class QLineEdit;
class QSignalMapper;

namespace caret {

    class CiftiMappableConnectivityMatrixDataFile;
    
    class CiftiConnectivityMatrixViewController : public QWidget, EventListenerInterface {
        
        Q_OBJECT

    public:
        CiftiConnectivityMatrixViewController(const Qt::Orientation orientation,
                                              QWidget* parent);
        
        virtual ~CiftiConnectivityMatrixViewController();
        
        void receiveEvent(Event* event);
        
    private slots:
        void enabledCheckBoxClicked(int);

    private:
        CiftiConnectivityMatrixViewController(const CiftiConnectivityMatrixViewController&);

        CiftiConnectivityMatrixViewController& operator=(const CiftiConnectivityMatrixViewController&);
        
        //void updateUserInterfaceAndGraphicsWindow();
        
        void updateViewController();
        
        void updateOtherCiftiConnectivityMatrixViewControllers();
        
        std::vector<QCheckBox*> m_fileEnableCheckBoxes;
        
        std::vector<QLineEdit*> m_fileNameLineEdits;
        
        QGridLayout* m_gridLayout;
        
        QSignalMapper* m_signalMapperFileEnableCheckBox;
        
        static std::set<CiftiConnectivityMatrixViewController*> s_allCiftiConnectivityMatrixViewControllers;
        
        static int COLUMN_ENABLE_CHECKBOX;
        static int COLUMN_COPY_BUTTON;
        static int COLUMN_NAME_LINE_EDIT;
        
    };
    
#ifdef __CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER_DECLARE__
    std::set<CiftiConnectivityMatrixViewController*> CiftiConnectivityMatrixViewController::s_allCiftiConnectivityMatrixViewControllers;
    int CiftiConnectivityMatrixViewController::COLUMN_ENABLE_CHECKBOX = 0;
    int CiftiConnectivityMatrixViewController::COLUMN_COPY_BUTTON     = 1;
    int CiftiConnectivityMatrixViewController::COLUMN_NAME_LINE_EDIT  = 2;
#endif // __CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CIFTI_CONNECTIVITY_MATRIX_VIEW_CONTROLLER__H_
