#ifndef __ANNOTATION_SELECTION_VIEW_CONTROLLER_H__
#define __ANNOTATION_SELECTION_VIEW_CONTROLLER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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


#include <QWidget>

#include "EventListenerInterface.h"
#include "SceneableInterface.h"

class QCheckBox;

namespace caret {
    class SceneClassAssistant;

    class AnnotationSelectionViewController : public QWidget, public EventListenerInterface, public SceneableInterface {
        Q_OBJECT
        
    public:
        AnnotationSelectionViewController(const int32_t browserWindowIndex,
                                          QWidget* parent);
        
        virtual ~AnnotationSelectionViewController();
        

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implemetation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private slots:
        void checkBoxToggled();
        
    private:
        AnnotationSelectionViewController(const AnnotationSelectionViewController&);

        AnnotationSelectionViewController& operator=(const AnnotationSelectionViewController&);
        
        SceneClassAssistant* m_sceneAssistant;

        int32_t m_browserWindowIndex;
        
        QCheckBox* m_displayModelAnnotationCheckBox;
        
        QCheckBox* m_displaySurfaceAnnotationCheckBox;
        
        QCheckBox* m_displayTabAnnotationCheckBox;
        
        QCheckBox* m_displayWindowAnnotationCheckBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_SELECTION_VIEW_CONTROLLER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__ANNOTATION_SELECTION_VIEW_CONTROLLER_H__
