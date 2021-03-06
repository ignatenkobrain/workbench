#ifndef __ANNOTATION_H__
#define __ANNOTATION_H__

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

#include "AnnotationAttributesDefaultTypeEnum.h"
#include "AnnotationCoordinateSpaceEnum.h"
#include "AnnotationSizingHandleTypeEnum.h"
#include "AnnotationTypeEnum.h"
#include "CaretColorEnum.h"
#include "CaretObjectTracksModification.h"
#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class Annotation : public CaretObjectTracksModification, public SceneableInterface {
        
    public:
        Annotation(const AnnotationTypeEnum::Enum type,
                   const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~Annotation();
        
        Annotation(const Annotation& obj);

        Annotation& operator=(const Annotation& obj);
       
        static Annotation* newAnnotationOfType(const AnnotationTypeEnum::Enum annotationType,
                                               const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        Annotation* clone() const;
        
        void replaceWithCopyOfAnnotation(const Annotation* annotation);
        
        QString getShortDescriptiveString() const;
        
        AnnotationTypeEnum::Enum getType() const ;
        
        AnnotationCoordinateSpaceEnum::Enum getCoordinateSpace() const;
        
        void setCoordinateSpace(const AnnotationCoordinateSpaceEnum::Enum coordinateSpace);
        
        int32_t getTabIndex() const;
        
        void setTabIndex(const int32_t tabIndex);
        
        int32_t getWindowIndex() const;
        
        void setWindowIndex(const int32_t windowIndex);
        
        CaretColorEnum::Enum getForegroundColor() const;

        void setForegroundColor(const CaretColorEnum::Enum color);

        void getForegroundColorRGBA(float rgbaOut[4]) const;
        
        void getForegroundColorRGBA(uint8_t rgbaOut[4]) const;
        
        CaretColorEnum::Enum getBackgroundColor() const;
        
        void setBackgroundColor(const CaretColorEnum::Enum color);
        
        void getBackgroundColorRGBA(float rgbaOut[4]) const;
        
        void getBackgroundColorRGBA(uint8_t rgbaOut[4]) const;
        
        void getCustomForegroundColor(float rgbaOut[4]) const;
        
        void getCustomForegroundColor(uint8_t rgbaOut[4]) const;
        
        void setCustomForegroundColor(const float rgba[4]);
        
        void setCustomForegroundColor(const uint8_t rgba[4]);
        
        void getCustomBackgroundColor(float rgbaOut[4]) const;
        
        void getCustomBackgroundColor(uint8_t rgbaOut[4]) const;
        
        void setCustomBackgroundColor(const float rgba[4]);
        
        void setCustomBackgroundColor(const uint8_t rgba[4]);
        
        float getForegroundLineWidth() const;
        
        void setForegroundLineWidth(const float lineWidth);
        
        virtual bool isForegroundLineWidthSupported() const;
        
        virtual bool isBackgroundColorSupported() const;
        
        bool isSelected() const;
        
        static void setUserDefaultForegroundColor(const CaretColorEnum::Enum color);
        
        static void setUserDefaultBackgroundColor(const CaretColorEnum::Enum color);
        
        static void setUserDefaultCustomForegroundColor(const float rgba[4]);
        
        static void setUserDefaultCustomBackgroundColor(const float rgba[4]);
        
        static void setUserDefaultForegroundLineWidth(const float lineWidth);
        /**
         * Apply a move or resize operation received from the GUI.
         *
         * @param handleSelected
         *     Annotatoion handle that is being dragged by the user.
         * @param viewportWidth
         *     Width of viewport
         * @param viewportHeight
         *     Height of viewport
         * @param mouseX
         *     Mouse X-coordinate.
         * @param mouseY
         *     Mouse Y-coordinate.
         * @param mouseDX
         *     Change in mouse X-coordinate.
         * @param mouseDY
         *     Change in mouse Y-coordinate.
         */
        virtual void applyMoveOrResizeFromGUI(const AnnotationSizingHandleTypeEnum::Enum handleSelected,
                                              const float viewportWidth,
                                              const float viewportHeight,
                                              const float mouseX,
                                              const float mouseY,
                                              const float mouseDX,
                                              const float mouseDY) = 0;
        
        bool isMovableOrResizableFromGUI() const;
        
        virtual void applyCoordinatesSizeAndRotationFromOther(const Annotation* otherAnnotation) = 0;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass) = 0;

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperAnnotation(const Annotation& obj);

        void initializeAnnotationMembers();
        
        // private - AnnotationManager handles selection and allowing
        // public access to this method could cause improper selection status
        void setSelected(const bool selectedStatus) const;
        
        const AnnotationTypeEnum::Enum m_type;
        
    protected:
        const AnnotationAttributesDefaultTypeEnum::Enum m_attributeDefaultType;
        
    private:
        SceneClassAssistant* m_sceneAssistant;
        
        AnnotationCoordinateSpaceEnum::Enum  m_coordinateSpace;
        
        int32_t m_tabIndex;
        
        int32_t m_windowIndex;
        
        CaretColorEnum::Enum m_colorForeground;
        
        CaretColorEnum::Enum m_colorBackground;
        
        float m_customColorForeground[4];
        
        float m_customColorBackground[4];
        
        float m_foregroundLineWidth;

        mutable bool m_selectedFlag;
        
        // defaults
        static CaretColorEnum::Enum s_userDefaultColorForeground;
        
        static CaretColorEnum::Enum s_userDefaultColorBackground;
        
        static float s_userDefaultCustomColorForeground[4];
        
        static float s_userDefaultCustomColorBackground[4];
        
        static float s_userDefaultForegroundLineWidth;
        
        // ADD_NEW_MEMBERS_HERE

        friend class AnnotationFile;
        friend class AnnotationManager;
    };
    
#ifdef __ANNOTATION_DECLARE__
    CaretColorEnum::Enum Annotation::s_userDefaultColorForeground = CaretColorEnum::WHITE;
    
    CaretColorEnum::Enum Annotation::s_userDefaultColorBackground = CaretColorEnum::NONE;
    
    float Annotation::s_userDefaultCustomColorForeground[4] = { 1.0, 1.0, 1.0, 1.0 };
    
    float Annotation::s_userDefaultCustomColorBackground[4] = { 0.0, 0.0, 0.0, 1.0 };
    
    float Annotation::s_userDefaultForegroundLineWidth = 3.0;
#endif // __ANNOTATION_DECLARE__

} // namespace
#endif  //__ANNOTATION_H__
