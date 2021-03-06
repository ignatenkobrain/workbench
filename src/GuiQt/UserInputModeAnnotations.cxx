
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

#define __USER_INPUT_MODE_ANNOTATIONS_DECLARE__
#include "UserInputModeAnnotations.h"
#undef __USER_INPUT_MODE_ANNOTATIONS_DECLARE__

#include "AnnotationChangeCoordinateDialog.h"
#include "AnnotationCreateDialog.h"
#include "AnnotationCoordinate.h"
#include "AnnotationFile.h"
#include "AnnotationManager.h"
#include "AnnotationOneDimensionalShape.h"
#include "AnnotationRedoUndoCommand.h"
#include "AnnotationText.h"
#include "AnnotationTextEditorDialog.h"
#include "AnnotationTwoDimensionalShape.h"
#include "Brain.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWidget.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CursorEnum.h"
#include "CaretPreferences.h"
#include "EventAnnotationCreateNewType.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventIdentificationRequest.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "IdentificationManager.h"
#include "KeyEvent.h"
#include "ModelSurfaceMontage.h"
#include "MouseEvent.h"
#include "SelectionItemAnnotation.h"
#include "SelectionManager.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxel.h"
#include "SessionManager.h"
#include "Surface.h"
#include "SurfaceMontageViewport.h"
#include "UserInputModeAnnotationsContextMenu.h"
#include "UserInputModeAnnotationsWidget.h"
#include "WuQMessageBox.h"

using namespace caret;


/**
 * \class caret::UserInputModeAnnotations 
 * \brief Input mode processor for Annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
UserInputModeAnnotations::UserInputModeAnnotations(const int32_t windowIndex)
: UserInputModeView(UserInputModeAbstract::ANNOTATIONS),
m_browserWindowIndex(windowIndex),
m_annotationBeingEdited(NULL),
m_annotationUnderMouse(NULL),
m_annotationBeingDragged(NULL)
{
    m_allowMultipleSelectionModeFlag = true;
    m_mode = MODE_SELECT;
    m_modeNewAnnotationType = AnnotationTypeEnum::LINE;
    m_annotationUnderMouseSizeHandleType = AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE;
    
    m_newAnnotationCreatingWithMouseDrag.grabNew(NULL);
    
    m_annotationToolsWidget = new UserInputModeAnnotationsWidget(this,
                                                                 m_browserWindowIndex);
    setWidgetForToolBar(m_annotationToolsWidget);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_CREATE_NEW_TYPE);
}

/**
 * Destructor.
 */
UserInputModeAnnotations::~UserInputModeAnnotations()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
UserInputModeAnnotations::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_CREATE_NEW_TYPE) {
        EventAnnotationCreateNewType* annotationEvent = dynamic_cast<EventAnnotationCreateNewType*>(event);
        CaretAssert(annotationEvent);
        
        AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
        
        annotationManager->deselectAllAnnotations();
        resetAnnotationUnderMouse();
        
        m_modeNewAnnotationType = annotationEvent->getAnnotationType();
        setMode(MODE_NEW_WITH_CLICK);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}


/**
 * Called when 'this' user input receiver is set
 * to receive events.
 */
void
UserInputModeAnnotations::initialize()
{
    m_mode = MODE_SELECT;
    resetAnnotationUnderMouse();
}

/**
 * Called when 'this' user input receiver is no
 * longer set to receive events.
 */
void
UserInputModeAnnotations::finish()
{
    m_mode = MODE_SELECT;
    resetAnnotationUnderMouse();
}

void
UserInputModeAnnotations::resetAnnotationUnderMouse()
{
    m_annotationBeingEdited = NULL;
    m_annotationUnderMouse  = NULL;
    m_annotationUnderMouseSizeHandleType = AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE;
    m_annotationBeingDragged = NULL;
    m_annotationBeingDraggedHandleType = AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE;
}

/**
 * Called to update the input receiver for various events.
 */
void
UserInputModeAnnotations::update()
{
    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
UserInputModeAnnotations::toString() const
{
    return "UserInputModeBorders";
}

/**
 * @return the mode.
 */
UserInputModeAnnotations::Mode
UserInputModeAnnotations::getMode() const
{
    return m_mode;
}

/**
 * Set the mode.
 * @param mode
 *    New value for mode.
 */
void
UserInputModeAnnotations::setMode(const Mode mode)
{
    if (m_mode != mode) {
        m_mode = mode;
    }
}

/**
 * @return The cursor for display in the OpenGL widget.
 */
CursorEnum::Enum
UserInputModeAnnotations::getCursor() const
{
    
    CursorEnum::Enum cursor = CursorEnum::CURSOR_DEFAULT;
    
    switch (m_mode) {
        case MODE_NEW_WITH_CLICK:
            cursor = CursorEnum::CURSOR_CROSS;
            break;
        case MODE_NEW_WITH_DRAG:
            cursor = CursorEnum::CURSOR_CROSS;
            break;
        case MODE_SELECT:
            if (m_annotationUnderMouse != NULL) {
                cursor = CursorEnum::CURSOR_FOUR_ARROWS;
                
                switch (m_annotationUnderMouseSizeHandleType) {
                    case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM:
                        cursor = CursorEnum::CURSOR_RESIZE_VERTICAL;
                        break;
                    case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_LEFT:
                        cursor = CursorEnum::CURSOR_RESIZE_BOTTOM_LEFT_TOP_RIGHT;
                        break;
                    case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_RIGHT:
                        cursor = CursorEnum::CURSOR_RESIZE_BOTTOM_RIGHT_TOP_LEFT;
                        break;
                    case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_LEFT:
                        cursor = CursorEnum::CURSOR_RESIZE_HORIZONTAL;
                        break;
                    case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_RIGHT:
                        cursor = CursorEnum::CURSOR_RESIZE_HORIZONTAL;
                        break;
                    case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP:
                        cursor = CursorEnum::CURSOR_RESIZE_VERTICAL;
                        break;
                    case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_LEFT:
                        cursor = CursorEnum::CURSOR_RESIZE_BOTTOM_RIGHT_TOP_LEFT;
                        break;
                    case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_RIGHT:
                        cursor = CursorEnum::CURSOR_RESIZE_BOTTOM_LEFT_TOP_RIGHT;
                        break;
                    case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_END:
                        cursor = CursorEnum::CURSOR_RESIZE_BOTTOM_LEFT_TOP_RIGHT;
                        break;
                    case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START:
                        cursor = CursorEnum::CURSOR_RESIZE_BOTTOM_LEFT_TOP_RIGHT;
                        break;
                    case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE:
                        cursor = CursorEnum::CURSOR_FOUR_ARROWS;
                        break;
                    case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
                        cursor = CursorEnum::CURSOR_ROTATION;
                        break;
                }
            }
            break;
        case MODE_SET_COORDINATE_ONE:
            cursor = CursorEnum::CURSOR_CROSS;
            break;
        case MODE_SET_COORDINATE_TWO:
            cursor = CursorEnum::CURSOR_CROSS;
            break;
    }
    
    return cursor;
}

/**
 * Process a key press event
 *
 * @param keyEvent
 *     Key event information.
 */
void
UserInputModeAnnotations::keyPressEvent(const KeyEvent& keyEvent)
{
    const int32_t keyCode = keyEvent.getKeyCode();
    switch (keyCode) {
        case Qt::Key_Delete:
        {
            switch (m_mode) {
                case MODE_NEW_WITH_CLICK:
                    break;
                case MODE_NEW_WITH_DRAG:
                    break;
                case MODE_SELECT:
                {
                    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
                    std::vector<Annotation*> selectedAnnotations = annotationManager->getSelectedAnnotations();
                    if ( ! selectedAnnotations.empty()) {
                        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
                        undoCommand->setModeDeleteAnnotations(selectedAnnotations);
                        annotationManager->applyCommand(undoCommand);
                        
                        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
                        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
                    }
                }
                    break;
                case MODE_SET_COORDINATE_ONE:
                    break;
                case MODE_SET_COORDINATE_TWO:
                    break;
            }
        }
            break;
        case Qt::Key_Escape:
        {
            bool selectModeFlag = false;
            switch (m_mode) {
                case MODE_NEW_WITH_CLICK:
                    break;
                case MODE_NEW_WITH_DRAG:
                    break;
                case MODE_SELECT:
                    break;
                case MODE_SET_COORDINATE_ONE:
                    selectModeFlag = true;
                    break;
                case MODE_SET_COORDINATE_TWO:
                    selectModeFlag = true;
                    break;
            }
            if (selectModeFlag) {
                setMode(MODE_SELECT);
                EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
            }
        }
            break;
        case Qt::Key_Down:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        {
            if (m_annotationBeingEdited != NULL) {
                bool changeCoordFlag  = false;
                bool moveOnePixelFlag = false;
                switch (m_annotationBeingEdited->getCoordinateSpace()) {
                    case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                        changeCoordFlag = true;
                        break;
                    case AnnotationCoordinateSpaceEnum::PIXELS:
                        break;
                    case AnnotationCoordinateSpaceEnum::SURFACE:
                        break;
                    case AnnotationCoordinateSpaceEnum::TAB:
                        changeCoordFlag  = true;
                        moveOnePixelFlag = true;
                        break;
                    case AnnotationCoordinateSpaceEnum::WINDOW:
                        changeCoordFlag  = true;
                        moveOnePixelFlag = true;
                        break;
                }
                
                if (changeCoordFlag) {
                    float distanceX = 1.0;
                    float distanceY = 1.0;
                    if (moveOnePixelFlag) {
                        const float pixelHeight = keyEvent.getOpenGLWidget()->height();
                        const float pixelWidth  = keyEvent.getOpenGLWidget()->width();
                        distanceX = 1.0 / pixelWidth;
                        distanceY = 1.0 / pixelHeight;
                    }
                    if (keyEvent.isShiftKeyDownFlag()) {
                        const float multiplier = 10;
                        distanceX *= multiplier;
                        distanceY *= multiplier;
                    }
                    
                    float dx = 0.0;
                    float dy = 0.0;
                    switch (keyCode) {
                        case Qt::Key_Down:
                            dy = -distanceY;
                            break;
                        case Qt::Key_Left:
                            dx = -distanceX;
                            break;
                        case Qt::Key_Right:
                            dx = distanceX;
                            break;
                        case Qt::Key_Up:
                            dy = distanceY;
                            break;
                        default:
                            CaretAssert(0);
                            break;
                    }
                    
                    AnnotationOneDimensionalShape* oneDim = dynamic_cast<AnnotationOneDimensionalShape*>(m_annotationBeingEdited);
                    AnnotationTwoDimensionalShape* twoDim = dynamic_cast<AnnotationTwoDimensionalShape*>(m_annotationBeingEdited);
                    
                    {
                            bool surfaceFlag = false;
                            switch (m_annotationBeingEdited->getCoordinateSpace()) {
                                case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                                    break;
                                case AnnotationCoordinateSpaceEnum::PIXELS:
                                    break;
                                case AnnotationCoordinateSpaceEnum::SURFACE:
                                    surfaceFlag = true;
                                    break;
                                case AnnotationCoordinateSpaceEnum::TAB:
                                    break;
                                case AnnotationCoordinateSpaceEnum::WINDOW:
                                    break;
                            }
                            
                            if ( ! surfaceFlag) {
                                AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
                                std::vector<Annotation*> annotations;
                                annotations.push_back(m_annotationBeingEdited);
                                
                                if (oneDim != NULL) {
                                    AnnotationCoordinate startCoord = *oneDim->getStartCoordinate();
                                    float xyzStart[3];
                                    startCoord.getXYZ(xyzStart);
                                    xyzStart[0] += dx;
                                    xyzStart[1] += dy;
                                    startCoord.setXYZ(xyzStart);
                                    
                                    AnnotationCoordinate endCoord   = *oneDim->getEndCoordinate();
                                    float xyzEnd[3];
                                    endCoord.getXYZ(xyzEnd);
                                    xyzEnd[0] += dx;
                                    xyzEnd[1] += dy;
                                    endCoord.setXYZ(xyzEnd);
                                    
                                    undoCommand->setModeCoordinateOneAndTwo(startCoord, endCoord, annotations);
                                }
                                else if (twoDim != NULL) {
                                    AnnotationCoordinate coord = *twoDim->getCoordinate();
                                    float xyz[3];
                                    coord.getXYZ(xyz);
                                    xyz[0] += dx;
                                    xyz[1] += dy;
                                    coord.setXYZ(xyz);
                                    
                                    undoCommand->setModeCoordinateOne(coord,
                                                                      annotations);
                                }
                                else {
                                    CaretAssert(0);
                                }
                                
                                if ( ! keyEvent.isFirstKeyPressFlag()) {
                                    undoCommand->setMergeEnabled(true);
                                }
                                
                                AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager();
                                annMan->applyCommand(undoCommand);
                                
                                EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
                                EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
                            }
                            
                        }
                }
            }
        }
            break;
    }
}

/**
 * Process a mouse left drag with no keys down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftDrag(const MouseEvent& mouseEvent)
{
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();

    switch (m_mode) {
        case MODE_NEW_WITH_CLICK:
        {
            if (m_newAnnotationCreatingWithMouseDrag != NULL) {
                m_newAnnotationCreatingWithMouseDrag.grabNew(NULL);
            }
            
            m_newAnnotationCreatingWithMouseDrag.grabNew(new NewMouseDragCreateAnnotation(m_modeNewAnnotationType,
                                                                                          mouseEvent));
            m_mode = MODE_NEW_WITH_DRAG;
            return;
        }
            break;
        case MODE_NEW_WITH_DRAG:
            userDrawingAnnotationFromMouseDrag(mouseEvent);
            return;
            break;
        case MODE_SELECT:
            break;
        case MODE_SET_COORDINATE_ONE:
            break;
        case MODE_SET_COORDINATE_TWO:
            break;
    }
    
    AnnotationCoordinateSpaceEnum::Enum draggingCoordinateSpace = AnnotationCoordinateSpaceEnum::PIXELS;
    
    std::vector<Annotation*> selectedAnnotations = annotationManager->getSelectedAnnotations();
    const int32_t numSelectedAnnotations = static_cast<int32_t>(selectedAnnotations.size());
    
    bool draggingValid = false;
    if (numSelectedAnnotations == 1) {
        draggingCoordinateSpace = selectedAnnotations[0]->getCoordinateSpace();
        draggingValid = true;
    }
    else if (numSelectedAnnotations > 1) {
        if (m_annotationBeingDraggedHandleType == AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE) {
            bool allSameSpaceFlag = true;
            draggingCoordinateSpace = selectedAnnotations[0]->getCoordinateSpace();
            for (int32_t i = 1; i < numSelectedAnnotations; i++) {
                if (selectedAnnotations[i]->getCoordinateSpace() != draggingCoordinateSpace) {
                    allSameSpaceFlag = false;
                    break;
                }
            }
            
            if (allSameSpaceFlag) {
                draggingValid = true;
            }
        }
    }
    
    if (draggingValid) {
        BrainOpenGLViewportContent* vpContent = mouseEvent.getViewportContent();
        if (vpContent == NULL) {
            return;
        }
        
        
        CoordinateInformation coordInfo;
        UserInputModeAnnotations::getValidCoordinateSpacesFromXY(mouseEvent.getOpenGLWidget(),
                                                                 mouseEvent.getViewportContent(),
                                                                 mouseEvent.getX(),
                                                                 mouseEvent.getY(),
                                                                 coordInfo);

        float spaceOriginX = 0.0;
        float spaceOriginY = 0.0;
        float spaceWidth  = 0.0;
        float spaceHeight = 0.0;
        
        bool draggableCoordSpaceFlag = false;
        switch (draggingCoordinateSpace) {
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                if ((coordInfo.m_modelXYZValid)
                    && (numSelectedAnnotations == 1)) {
                    draggableCoordSpaceFlag = true;
                }
                break;
            case AnnotationCoordinateSpaceEnum::PIXELS:
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                if ((coordInfo.m_surfaceNodeValid)
                    && (numSelectedAnnotations == 1)) {
                    draggableCoordSpaceFlag = true;
                }
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
            {
                int viewport[4];
                vpContent->getTabViewport(viewport);
                spaceOriginX = viewport[0];
                spaceOriginY = viewport[1];
                spaceWidth   = viewport[2];
                spaceHeight  = viewport[3];
                draggableCoordSpaceFlag = true;
            }
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
            {
                int viewport[4];
                vpContent->getWindowViewport(viewport);
                spaceOriginX = viewport[0];
                spaceOriginY = viewport[1];
                spaceWidth   = viewport[2];
                spaceHeight  = viewport[3];
                draggableCoordSpaceFlag = true;
            }
                break;
        }
        
        if (draggableCoordSpaceFlag) {
            const float dx = mouseEvent.getDx();
            const float dy = mouseEvent.getDy();
            
            const float mouseViewportX = mouseEvent.getX() - spaceOriginX;
            const float mouseViewportY = mouseEvent.getY() - spaceOriginY;
            
            AnnotationRedoUndoCommand* command = new AnnotationRedoUndoCommand();
            std::vector<Annotation*> annotationsBeforeMoveAndResize;
            std::vector<Annotation*> annotationsAfterMoveAndResize;
            
            for (int32_t i = 0; i < numSelectedAnnotations; i++) {
                Annotation* annotationModified(selectedAnnotations[i]->clone());
                if (draggingCoordinateSpace == AnnotationCoordinateSpaceEnum::SURFACE) {
                    AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(annotationModified);
                    if (twoDimAnn != NULL) {
                        AnnotationCoordinate* coord = twoDimAnn->getCoordinate();
                        coord->setSurfaceSpace(coordInfo.m_surfaceStructure,
                                               coordInfo.m_surfaceNumberOfNodes,
                                               coordInfo.m_surfaceNodeIndex,
                                               coord->getSurfaceOffsetLength());
                    }
                }
                else if (draggingCoordinateSpace == AnnotationCoordinateSpaceEnum::STEREOTAXIC) {
                    AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(annotationModified);
                    if (twoDimAnn != NULL) {
                        AnnotationCoordinate* coord = twoDimAnn->getCoordinate();
                        coord->setXYZ(coordInfo.m_modelXYZ);
                    }
                }
                else {
                    annotationModified->applyMoveOrResizeFromGUI(m_annotationBeingDraggedHandleType,
                                                                 spaceWidth,
                                                                 spaceHeight,
                                                                 mouseViewportX,
                                                                 mouseViewportY,
                                                                 dx,
                                                                 dy);
                }
                
                annotationsBeforeMoveAndResize.push_back(selectedAnnotations[i]);
                annotationsAfterMoveAndResize.push_back(annotationModified);
            }
            
            command->setModeLocationAndSize(annotationsBeforeMoveAndResize,
                                            annotationsAfterMoveAndResize);
            
            if ( ! mouseEvent.isFirstDragging()) {
                command->setMergeEnabled(true);
            }
            annotationManager->applyCommand(command);
            
            for (std::vector<Annotation*>::iterator iter = annotationsAfterMoveAndResize.begin();
                 iter != annotationsAfterMoveAndResize.end();
                 iter++) {
                delete *iter;
            }
            annotationsAfterMoveAndResize.clear();
            
            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
            m_annotationToolsWidget->updateWidget();
        }
    }
}

/**
 * Process a mouse left drag with only the alt key down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftDragWithAlt(const MouseEvent& /*mouseEvent*/)
{
}

/**
 * Process a mouse left drag with ctrl key down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftDragWithCtrl(const MouseEvent& /*mouseEvent*/) { }

/**
 * Process a mouse left drag with ctrl and shift keys down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftDragWithCtrlShift(const MouseEvent& /*mouseEvent*/) { }

/**
 * Process a mouse left drag with shift key down event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftDragWithShift(const MouseEvent& mouseEvent)
{
    mouseLeftDrag(mouseEvent);
}

/**
 * Process a mouse left click event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftClick(const MouseEvent& mouseEvent)
{
    switch (m_mode) {
        case MODE_NEW_WITH_CLICK:
            processModeNewMouseLeftClick(mouseEvent);
            break;
        case MODE_NEW_WITH_DRAG:
            break;
        case MODE_SELECT:
            break;
        case MODE_SET_COORDINATE_ONE:
            processModeSetCoordinate(mouseEvent);
            break;
        case MODE_SET_COORDINATE_TWO:
            processModeSetCoordinate(mouseEvent);
            break;
    }
}

/**
 * Process a mouse left click with Shift key event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftClickWithShift(const MouseEvent& mouseEvent)
{
    switch (m_mode) {
        case MODE_NEW_WITH_CLICK:
            break;
        case MODE_NEW_WITH_DRAG:
            break;
        case MODE_SELECT:
            if (m_allowMultipleSelectionModeFlag) {
                processMouseSelectAnnotation(mouseEvent,
                                             true);
            }
            break;
        case MODE_SET_COORDINATE_ONE:
            break;
        case MODE_SET_COORDINATE_TWO:
            break;
    }
}

/**
 * Process a mouse left press event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftPress(const MouseEvent& mouseEvent)
{
    switch (m_mode) {
        case MODE_NEW_WITH_CLICK:
            break;
        case MODE_NEW_WITH_DRAG:
            break;
        case MODE_SELECT:
            processMouseSelectAnnotation(mouseEvent,
                                         false);
            break;
        case MODE_SET_COORDINATE_ONE:
            break;
        case MODE_SET_COORDINATE_TWO:
            break;
    }
}

/**
 * Set the annotation under the mouse that results in update of the cursor.
 *
 * @param mouseEvent
 *     Mouse event information.
 * @param annotationIDIn
 *     Optional (if not NULL) annotation ID that may be provided by caller
 *     and can be used to avoid an identification operation.
 */
void
UserInputModeAnnotations::setAnnotationUnderMouse(const MouseEvent& mouseEvent,
                                                  SelectionItemAnnotation* annotationIDIn)
{
    m_annotationUnderMouse = NULL;
    m_annotationUnderMouseSizeHandleType = AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE;
    
    
    BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
    SelectionItemAnnotation* annotationID = annotationIDIn;
    if (annotationID == NULL) {
        annotationID = openGLWidget->performIdentificationAnnotations(mouseEvent.getX(),
                                                                      mouseEvent.getY());
    }
    if (annotationID->isValid()) {
        m_annotationUnderMouse = annotationID->getAnnotation();
        m_annotationUnderMouseSizeHandleType = annotationID->getSizingHandle();
    }
    
    openGLWidget->updateCursor();
}

/**
 * User drawing a new annotation from dragging the mouse from corner/end to another
 * corner/end.
 *
 * @param mouseEvent
 *     Mouse event issued when mouse button was released.
 */
void
UserInputModeAnnotations::userDrawingAnnotationFromMouseDrag(const MouseEvent& mouseEvent)
{
    if (m_newAnnotationCreatingWithMouseDrag != NULL) {
        m_newAnnotationCreatingWithMouseDrag->update(mouseEvent.getX(),
                                                     mouseEvent.getY());

        AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
        annotationManager->setAnnotationBeingDrawnInWindow(m_browserWindowIndex,
                                                           m_newAnnotationCreatingWithMouseDrag->getAnnotation());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * Create a new annotation from dragging the mouse from corner/end to another
 * corner/end.
 *
 * @param mouseEvent
 *     Mouse event issued when mouse button was released.
 */
void
UserInputModeAnnotations::createNewAnnotationFromMouseDrag(const MouseEvent& mouseEvent)
{
    if (m_newAnnotationCreatingWithMouseDrag != NULL) {
        
        CaretPointer<AnnotationCreateDialog> annotationDialog(AnnotationCreateDialog::newAnnotationTypeWithBounds(mouseEvent,
                                                                                                                  m_newAnnotationCreatingWithMouseDrag->getAnnotation()->getType(),
                                                                                                                  mouseEvent.getOpenGLWidget()));
        if (annotationDialog->exec() == AnnotationCreateDialog::Accepted) {
            AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
            const std::vector<Annotation*> allSelectedAnnotations = annotationManager->getSelectedAnnotations();
            if (allSelectedAnnotations.size() == 1) {
                selecteAnnotation(allSelectedAnnotations[0]);
            }
        }
        
        m_newAnnotationCreatingWithMouseDrag.grabNew(NULL);
        
        AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
        annotationManager->setAnnotationBeingDrawnInWindow(m_browserWindowIndex,
                                                           NULL);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}


/**
 * Process a mouse left release event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftRelease(const MouseEvent& mouseEvent)
{
    switch (m_mode) {
        case MODE_NEW_WITH_CLICK:
            break;
        case MODE_NEW_WITH_DRAG:
            createNewAnnotationFromMouseDrag(mouseEvent);
            m_mode = MODE_SELECT;
            break;
        case MODE_SELECT:
            break;
        case MODE_SET_COORDINATE_ONE:
            break;
        case MODE_SET_COORDINATE_TWO:
            break;
    }
    
    m_annotationBeingDragged = NULL;
    
    setAnnotationUnderMouse(mouseEvent,
                            NULL);
}

/**
 * Process a mouse left double-click event.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseLeftDoubleClick(const MouseEvent& mouseEvent)
{
    const int32_t mouseX = mouseEvent.getX();
    const int32_t mouseY = mouseEvent.getY();
    
    BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
    SelectionItemAnnotation* annotationID = openGLWidget->performIdentificationAnnotations(mouseX,
                                                                            mouseY);
    if (annotationID->isValid()) {
        Annotation* annotation = annotationID->getAnnotation();
        if (annotation != NULL) {
            AnnotationText* textAnnotation = dynamic_cast<AnnotationText*>(annotation);
            if (textAnnotation != NULL) {
                AnnotationTextEditorDialog ted(textAnnotation,
                                               openGLWidget);
                /*
                 * Note: Y==0 is at top for widget.
                 *       Y==0 is at bottom for OpenGL mouse x,y
                 */
                ted.move(openGLWidget->mapToGlobal(QPoint(mouseX,
                                                          (openGLWidget->height() - mouseY + 20))));
                ted.exec();
                EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
            }
        }
    }
}


/**
 * Process a mouse move with no buttons or keys down
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseMove(const MouseEvent& mouseEvent)
{
    setAnnotationUnderMouse(mouseEvent,
                            NULL);
}

/**
 * Process a mouse move with no buttons and shift key down
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::mouseMoveWithShift(const MouseEvent& mouseEvent)
{
    setAnnotationUnderMouse(mouseEvent,
                            NULL);
}

/**
 * Get the different types of coordinates at the given mouse location.
 *
 * @param openGLWidget
 *     The OpenGL Widget.
 * @param viewportContent
 *     The content of the viewport.
 * @param windowX
 *     X-coordinate in the window.
 * @param windowY
 *     Y-coordinate in the window.
 * @param coordInfoOut
 *     Output containing coordinate information.
 */
void
UserInputModeAnnotations::getValidCoordinateSpacesFromXY(BrainOpenGLWidget* openGLWidget,
                                                         BrainOpenGLViewportContent* viewportContent,
                                                         const int32_t windowX,
                                                         const int32_t windowY,
                                                         CoordinateInformation& coordInfoOut)
{
    coordInfoOut.reset();
    
    SelectionManager* idManager =
    openGLWidget->performIdentification(windowX,
                                        windowY,
                                        false);
    
    SelectionItemVoxel* voxelID = idManager->getVoxelIdentification();
    SelectionItemSurfaceNode*  surfaceNodeIdentification = idManager->getSurfaceNodeIdentification();
    if (surfaceNodeIdentification->isValid()) {
        surfaceNodeIdentification->getModelXYZ(coordInfoOut.m_modelXYZ);
        coordInfoOut.m_modelXYZValid = true;
        
        const Surface* surface = surfaceNodeIdentification->getSurface();
        CaretAssert(surface);
        coordInfoOut.m_surfaceNumberOfNodes = surface->getNumberOfNodes();
        coordInfoOut.m_surfaceStructure     = surface->getStructure();
        coordInfoOut.m_surfaceNodeIndex     = surfaceNodeIdentification->getNodeNumber();
        coordInfoOut.m_surfaceNodeOffset    = AnnotationCoordinate::getDefaultSurfaceOffsetLength();
        coordInfoOut.m_surfaceNodeValid     = true;
    }
    else if (voxelID->isValid()) {
        voxelID->getModelXYZ(coordInfoOut.m_modelXYZ);
        coordInfoOut.m_modelXYZValid = true;
    }
    
    /*
     * In tile tabs, some regions may not contain a tab such
     * as three tabs in a two-by-two configuration
     * or if the user has clicked in a margin
     */
    BrowserTabContent* tabContent = viewportContent->getBrowserTabContent();
    if (tabContent != NULL) {
        int tabViewport[4];
        viewportContent->getModelViewport(tabViewport);
        const float tabX = 100.0 * (windowX - tabViewport[0]) / static_cast<float>(tabViewport[2]);
        const float tabY = 100.0 * (windowY - tabViewport[1]) / static_cast<float>(tabViewport[3]);
        if ((tabX >= 0.0)
            && (tabX < 100.0)
            && (tabY >= 0.0)
            && (tabY <= 100.0)) {
            coordInfoOut.m_tabXYZ[0] = tabX;
            coordInfoOut.m_tabXYZ[1] = tabY;
            coordInfoOut.m_tabXYZ[2] = 0.0;
            coordInfoOut.m_tabIndex  = tabContent->getTabNumber();
            coordInfoOut.m_tabWidth  = tabViewport[2];
            coordInfoOut.m_tabHeight = tabViewport[3];
        }
    }
    
    int windowViewport[4];
    viewportContent->getWindowViewport(windowViewport);
//    coordInfoOut.m_windowXYZ[0] = windowViewport[0] + windowX;
//    coordInfoOut.m_windowXYZ[1] = windowViewport[1] + windowY;
    coordInfoOut.m_windowXYZ[0] = windowX - windowViewport[0];
    coordInfoOut.m_windowXYZ[1] = windowY - windowViewport[1];
    coordInfoOut.m_windowXYZ[2] = 0.0;
    coordInfoOut.m_windowIndex  = viewportContent->getWindowIndex();
    coordInfoOut.m_windowWidth  = windowViewport[2];
    coordInfoOut.m_windowHeight = windowViewport[3];
    
    /*
     * Normalize window coordinates (width and height range [0, 100]
     */
    coordInfoOut.m_windowXYZ[0] = 100.0 * (coordInfoOut.m_windowXYZ[0] / windowViewport[2]);
    coordInfoOut.m_windowXYZ[1] = 100.0 * (coordInfoOut.m_windowXYZ[1] / windowViewport[3]);
}

/**
 * Set the coordinates for the annotation.
 *
 * @param annotation
 *     The annotation.
 * @param coordinateSpace
 *     The coordinate space.
 * @parm coordInfoOne
 *     Data for the first coordinate.
 * @parm coordInfoTwo
 *     Data for the second coordinate.
 */
bool
UserInputModeAnnotations::setAnnotationCoordinatesForSpace(Annotation* annotation,
                                                           const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                                                           const CoordinateInformation* coordInfoOne,
                                                           const CoordinateInformation* coordInfoTwo)
{
    CaretAssert(annotation);
    
    bool validCoordinateFlag = false;

    AnnotationOneDimensionalShape* oneDimAnn = dynamic_cast<AnnotationOneDimensionalShape*>(annotation);
    AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(annotation);
    
    if (oneDimAnn != NULL) {
        validCoordinateFlag = setOneDimAnnotationCoordinatesForSpace(oneDimAnn,
                                               coordinateSpace,
                                               coordInfoOne,
                                               coordInfoTwo);
    }
    else if (twoDimAnn != NULL) {
        validCoordinateFlag = setTwoDimAnnotationCoordinatesForSpace(twoDimAnn,
                                               coordinateSpace,
                                               coordInfoOne,
                                               coordInfoTwo);
    }
    
    return validCoordinateFlag;
}

/**
 * Set the coordinates for the one-dimensional annotation.
 *
 * @param annotation
 *     The annotation.
 * @param coordinateSpace
 *     The coordinate space.
 * @parm coordInfoOne
 *     Data for the first coordinate.
 * @parm coordInfoTwo
 *     Data for the second coordinate.
 */
bool
UserInputModeAnnotations::setOneDimAnnotationCoordinatesForSpace(AnnotationOneDimensionalShape* annotation,
                                                   const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                                                   const CoordinateInformation* coordInfoOne,
                                                   const CoordinateInformation* coordInfoTwo)
{
    bool validCoordinateFlag = false;
    
    CaretAssert(annotation);
    CaretAssert(coordInfoOne);
    
    AnnotationCoordinate* startCoordinate = annotation->getStartCoordinate();
    CaretAssert(startCoordinate);
    AnnotationCoordinate* endCoordinate   = annotation->getEndCoordinate();
    CaretAssert(endCoordinate);
    
    switch (coordinateSpace) {
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            if (coordInfoOne->m_modelXYZValid) {
                startCoordinate->setXYZ(coordInfoOne->m_modelXYZ);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::STEREOTAXIC);
                
                validCoordinateFlag = true;
                
                if (coordInfoTwo != NULL) {
                    if (coordInfoTwo->m_modelXYZValid) {
                        if (endCoordinate != NULL) {
                            endCoordinate->setXYZ(coordInfoTwo->m_modelXYZ);
                        }
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::PIXELS:
            CaretAssert(0);
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            if (coordInfoOne->m_surfaceNodeValid) {
                startCoordinate->setSurfaceSpace(coordInfoOne->m_surfaceStructure,
                                                 coordInfoOne->m_surfaceNumberOfNodes,
                                                 coordInfoOne->m_surfaceNodeIndex,
                                                 coordInfoOne->m_surfaceNodeOffset);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SURFACE);
                
                validCoordinateFlag = true;
                
                if (coordInfoTwo != NULL) {
                    if (coordInfoTwo->m_surfaceNodeValid) {
                        if (endCoordinate != NULL) {
                            endCoordinate->setSurfaceSpace(coordInfoTwo->m_surfaceStructure,
                                                           coordInfoTwo->m_surfaceNumberOfNodes,
                                                           coordInfoTwo->m_surfaceNodeIndex,
                                                           coordInfoTwo->m_surfaceNodeOffset);
                        }
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            if (coordInfoOne->m_tabIndex >= 0) {
                startCoordinate->setXYZ(coordInfoOne->m_tabXYZ);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
                annotation->setTabIndex(coordInfoOne->m_tabIndex);
                
                validCoordinateFlag = true;
                
                
                if (coordInfoTwo != NULL) {
                    if (coordInfoTwo->m_tabIndex >= 0) {
                        if (endCoordinate != NULL) {
                            endCoordinate->setXYZ(coordInfoTwo->m_tabXYZ);
                        }
                    }
                }
                else if (endCoordinate != NULL) {
                    double xyz[3] = {
                        coordInfoOne->m_tabXYZ[0],
                        coordInfoOne->m_tabXYZ[1],
                        coordInfoOne->m_tabXYZ[2]
                    };
                    if (xyz[1] > 50.0) {
                        xyz[1] -= 25.0;
                    }
                    else {
                        xyz[1] += 25.0;
                    }
                    endCoordinate->setXYZ(xyz);
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            if (coordInfoOne->m_windowIndex >= 0) {
                startCoordinate->setXYZ(coordInfoOne->m_windowXYZ);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
                annotation->setWindowIndex(coordInfoOne->m_windowIndex);
                
                validCoordinateFlag = true;
                
                if (coordInfoTwo != NULL) {
                    if (coordInfoTwo->m_windowIndex >= 0) {
                        if (endCoordinate != NULL) {
                            endCoordinate->setXYZ(coordInfoTwo->m_windowXYZ);
                        }
                    }
                }
                else if (endCoordinate != NULL) {
                    double xyz[3] = {
                        coordInfoOne->m_windowXYZ[0],
                        coordInfoOne->m_windowXYZ[1],
                        coordInfoOne->m_windowXYZ[2]
                    };
                    if (xyz[1] > 50.0) {
                        xyz[1] -= 25.0;
                    }
                    else {
                        xyz[1] += 25.0;
                    }
                    endCoordinate->setXYZ(xyz);
                }
            }
            break;
    }

    return validCoordinateFlag;
}

/**
 * Set the coordinates for the two-dimensional annotation.
 * If both coordinates are valid (not NULL), the annotation is
 * placed at the average of the two coordinates.  Otherwise,
 * the annotation is placed at the first coordinate.
 *
 * @param annotation
 *     The annotation.
 * @param coordinateSpace
 *     The coordinate space.
 * @parm coordInfoOne
 *     Data for the first coordinate.
 * @parm optionalCoordInfoTwo
 *     Data for the optional second coordinate.
 */
bool
UserInputModeAnnotations::setTwoDimAnnotationCoordinatesForSpace(AnnotationTwoDimensionalShape* annotation,
                                                   const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                                                   const CoordinateInformation* coordInfoOne,
                                                   const CoordinateInformation* optionalCoordInfoTwo)
{
    bool validCoordinateFlag = false;
    
    CaretAssert(annotation);
    CaretAssert(coordInfoOne);
    
    bool setWidthHeightWithTabCoordsFlag    = false;
    bool setWidthHeightWithWindowCoordsFlag = false;
    
    AnnotationCoordinate* coordinate = annotation->getCoordinate();
    
    switch (coordinateSpace) {
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            if (coordInfoOne->m_modelXYZValid) {
                coordinate->setXYZ(coordInfoOne->m_modelXYZ);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::STEREOTAXIC);
                
                validCoordinateFlag = true;
                
                if (optionalCoordInfoTwo != NULL) {
                    if (optionalCoordInfoTwo->m_modelXYZValid) {
                        float centerXYZ[3] = {
                            (coordInfoOne->m_modelXYZ[0] + optionalCoordInfoTwo->m_modelXYZ[0]) / 2.0,
                            (coordInfoOne->m_modelXYZ[1] + optionalCoordInfoTwo->m_modelXYZ[1]) / 2.0,
                            (coordInfoOne->m_modelXYZ[2] + optionalCoordInfoTwo->m_modelXYZ[2]) / 2.0
                        };
                        coordinate->setXYZ(centerXYZ);
                        setWidthHeightWithTabCoordsFlag = true;
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::PIXELS:
            CaretAssert(0);
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            if (coordInfoOne->m_surfaceNodeValid) {
                coordinate->setSurfaceSpace(coordInfoOne->m_surfaceStructure,
                                            coordInfoOne->m_surfaceNumberOfNodes,
                                            coordInfoOne->m_surfaceNodeIndex,
                                            coordInfoOne->m_surfaceNodeOffset);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SURFACE);
                
                validCoordinateFlag = true;
                
                if (optionalCoordInfoTwo != NULL) {
                    if ((optionalCoordInfoTwo->m_surfaceNodeValid)
                        && (optionalCoordInfoTwo->m_surfaceStructure == coordInfoOne->m_surfaceStructure)) {
                        if ((optionalCoordInfoTwo->m_windowIndex == coordInfoOne->m_windowIndex)
                            && (coordInfoOne->m_windowIndex >= 0)) {
                            const float windowWidth  = coordInfoOne->m_windowWidth;
                            const float windowHeight = coordInfoOne->m_windowHeight;
                            const float x1 = coordInfoOne->m_windowXYZ[0] * windowWidth;
                            const float y1 = coordInfoOne->m_windowXYZ[1] * windowHeight;
                            const float x2 = optionalCoordInfoTwo->m_windowXYZ[0] * windowWidth;
                            const float y2 = optionalCoordInfoTwo->m_windowXYZ[1] * windowHeight;
                            const int32_t windowX = static_cast<int32_t>((x1 + x2)) / 2.0;
                            const int32_t windowY = static_cast<int32_t>((y1 + y2)) / 2.0;
                            
                            EventIdentificationRequest idRequest(coordInfoOne->m_windowIndex,
                                                                 static_cast<int32_t>(windowX),
                                                                 static_cast<int32_t>(windowY));
                            EventManager::get()->sendEvent(idRequest.getPointer());
                            SelectionManager* sm = idRequest.getSelectionManager();
                            if (sm != NULL) {
                                const SelectionItemSurfaceNode* nodeID = sm->getSurfaceNodeIdentification();
                                CaretAssert(nodeID);
                                if (nodeID->isValid()) {
                                    if (nodeID->getSurface()->getStructure() == coordInfoOne->m_surfaceStructure) {
                                        coordinate->setSurfaceSpace(coordInfoOne->m_surfaceStructure,
                                                                    coordInfoOne->m_surfaceNumberOfNodes,
                                                                    nodeID->getNodeNumber(),
                                                                    0.0);
                                        setWidthHeightWithTabCoordsFlag = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            if (coordInfoOne->m_tabIndex >= 0) {
                coordinate->setXYZ(coordInfoOne->m_tabXYZ);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
                annotation->setTabIndex(coordInfoOne->m_tabIndex);
                
                validCoordinateFlag = true;
                
                if (optionalCoordInfoTwo != NULL) {
                    if (optionalCoordInfoTwo->m_tabIndex == coordInfoOne->m_tabIndex) {
                        float centerXYZ[3] = {
                            (coordInfoOne->m_tabXYZ[0] + optionalCoordInfoTwo->m_tabXYZ[0]) / 2.0,
                            (coordInfoOne->m_tabXYZ[1] + optionalCoordInfoTwo->m_tabXYZ[1]) / 2.0,
                            (coordInfoOne->m_tabXYZ[2] + optionalCoordInfoTwo->m_tabXYZ[2]) / 2.0
                        };
                        coordinate->setXYZ(centerXYZ);
                        setWidthHeightWithTabCoordsFlag = true;
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            if (coordInfoOne->m_windowIndex >= 0) {
                coordinate->setXYZ(coordInfoOne->m_windowXYZ);
                annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
                annotation->setWindowIndex(coordInfoOne->m_windowIndex);
                
                validCoordinateFlag = true;
                
                if (optionalCoordInfoTwo != NULL) {
                    if (optionalCoordInfoTwo->m_windowIndex == coordInfoOne->m_windowIndex) {
                        float centerXYZ[3] = {
                            (coordInfoOne->m_windowXYZ[0] + optionalCoordInfoTwo->m_windowXYZ[0]) / 2.0,
                            (coordInfoOne->m_windowXYZ[1] + optionalCoordInfoTwo->m_windowXYZ[1]) / 2.0,
                            (coordInfoOne->m_windowXYZ[2] + optionalCoordInfoTwo->m_windowXYZ[2]) / 2.0
                        };
                        coordinate->setXYZ(centerXYZ);
                        setWidthHeightWithWindowCoordsFlag = true;
                    }
                }
            }
            break;
    }
    
    if (setWidthHeightWithTabCoordsFlag) {
        if (coordInfoOne->m_tabIndex >= 0) {
            if (optionalCoordInfoTwo != NULL) {
                if (coordInfoOne->m_tabIndex == optionalCoordInfoTwo->m_tabIndex) {
                    const float tabWidth  = coordInfoOne->m_tabWidth;
                    const float tabHeight = coordInfoOne->m_tabHeight;
                    
                    const float oneXYZ[3] = {
                        coordInfoOne->m_tabXYZ[0],
                        coordInfoOne->m_tabXYZ[1],
                        coordInfoOne->m_tabXYZ[2]
                    };
                    const float twoXYZ[3] = {
                        optionalCoordInfoTwo->m_tabXYZ[0],
                        optionalCoordInfoTwo->m_tabXYZ[1],
                        optionalCoordInfoTwo->m_tabXYZ[2]
                    };
                    
                    annotation->setWidthAndHeightFromBounds(oneXYZ,
                                                            twoXYZ,
                                                            tabWidth,
                                                            tabHeight);
                }
            }
        }
    }
    else if (setWidthHeightWithWindowCoordsFlag) {
        if (coordInfoOne->m_windowIndex >= 0) {
            if (optionalCoordInfoTwo != NULL) {
                if (coordInfoOne->m_windowIndex == optionalCoordInfoTwo->m_windowIndex) {
                    const float windowWidth  = coordInfoOne->m_windowWidth;
                    const float windowHeight = coordInfoOne->m_windowHeight;
                    
                    const float oneXYZ[3] = {
                        coordInfoOne->m_windowXYZ[0],
                        coordInfoOne->m_windowXYZ[1],
                        coordInfoOne->m_windowXYZ[2]
                    };
                    const float twoXYZ[3] = {
                        optionalCoordInfoTwo->m_windowXYZ[0],
                        optionalCoordInfoTwo->m_windowXYZ[1],
                        optionalCoordInfoTwo->m_windowXYZ[2]
                    };
                    
                    annotation->setWidthAndHeightFromBounds(oneXYZ,
                                                            twoXYZ,
                                                            windowWidth,
                                                            windowHeight);
                }
            }
        }
    }
    
    return validCoordinateFlag;
}


/**
 * Process a mouse left click to set a coordinate.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::processModeSetCoordinate(const MouseEvent& mouseEvent)
{
    if (m_annotationBeingEdited == NULL) {
        return;
    }

    CoordinateInformation coordInfo;
    UserInputModeAnnotations::getValidCoordinateSpacesFromXY(mouseEvent.getOpenGLWidget(),
                                                             mouseEvent.getViewportContent(),
                                                             mouseEvent.getX(),
                                                             mouseEvent.getY(),
                                                             coordInfo);
    
    AnnotationOneDimensionalShape* oneDimAnn = dynamic_cast<AnnotationOneDimensionalShape*>(m_annotationBeingEdited);
    AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(m_annotationBeingEdited);

    AnnotationCoordinate* coordinate = NULL;
    AnnotationCoordinate* otherCoordinate = NULL;
    switch (m_mode) {
        case MODE_NEW_WITH_CLICK:
            break;
        case MODE_NEW_WITH_DRAG:
            break;
        case MODE_SELECT:
            break;
        case MODE_SET_COORDINATE_ONE:
            if (oneDimAnn != NULL) {
                coordinate      = oneDimAnn->getStartCoordinate();
                otherCoordinate = oneDimAnn->getEndCoordinate();
            }
            else if (twoDimAnn != NULL) {
                coordinate = twoDimAnn->getCoordinate();
            }
            break;
        case MODE_SET_COORDINATE_TWO:
            if (oneDimAnn != NULL) {
                coordinate      = oneDimAnn->getEndCoordinate();
                otherCoordinate = oneDimAnn->getStartCoordinate();
            }
            break;
    }
    
    if (coordinate != NULL) {
        StructureEnum::Enum structure = StructureEnum::INVALID;
        int32_t numNodes = -1;
        int32_t nodeIndex = -1;
        float surfaceOffset = 0.0;
        coordinate->getSurfaceSpace(structure, numNodes, nodeIndex, surfaceOffset);
        coordInfo.m_surfaceNodeOffset = surfaceOffset;
    }
    
    AnnotationChangeCoordinateDialog changeCoordDialog(coordInfo,
                                                       m_annotationBeingEdited,
                                                       coordinate,
                                                       otherCoordinate,
                                                       m_annotationToolsWidget);
    if (changeCoordDialog.exec() == AnnotationChangeCoordinateDialog::Accepted) {
        
    }

    setMode(MODE_SELECT);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
}


/**
 * Process a mouse left click for new mode.
 *
 * @param mouseEvent
 *     Mouse event information.
 */
void
UserInputModeAnnotations::processModeNewMouseLeftClick(const MouseEvent& mouseEvent)
{
    resetAnnotationUnderMouse();
    
    CaretPointer<AnnotationCreateDialog> annotationDialog(AnnotationCreateDialog::newAnnotationType(mouseEvent,
                                                                                                    m_modeNewAnnotationType,
                                                                                                    mouseEvent.getOpenGLWidget()));
    if (annotationDialog->exec() == AnnotationCreateDialog::Accepted) {
        AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
        const std::vector<Annotation*> allSelectedAnnotations = annotationManager->getSelectedAnnotations();
        if (allSelectedAnnotations.size() == 1) {
            selecteAnnotation(allSelectedAnnotations[0]);
        }
    }

    setMode(MODE_SELECT);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
}

/**
 * Select the given annotation (typically when a new annontation is created).
 *
 * @param annotation
 *    Annotation that is selected.
 */
void
UserInputModeAnnotations::selecteAnnotation(Annotation* annotation)
{
    resetAnnotationUnderMouse();
    
    m_annotationBeingEdited  = annotation;
    m_annotationBeingDragged = annotation;
    m_annotationUnderMouse   = annotation;
}


/**
 * Process a mouse left click for selection mode.
 *
 * @param mouseEvent
 *     Mouse event information.
 * @param shiftKeyDownFlag
 *     True if shift key is down.
 */
void
UserInputModeAnnotations::processMouseSelectAnnotation(const MouseEvent& mouseEvent,
                                                       const bool shiftKeyDownFlag)
{
    BrainOpenGLWidget* openGLWidget = mouseEvent.getOpenGLWidget();
    const int mouseX = mouseEvent.getX();
    const int mouseY = mouseEvent.getY();
    
    m_annotationBeingEdited  = NULL;
    m_annotationBeingDragged = NULL;
    
    /*
     * NOTE: When selecting annotations:
     *    (A) When the mouse is clicked WITHOUT the SHIFT key down, the user is in
     *        'single-annotation-selection-mode' and at most one annotation will
     *        be selected when this method completes.
     *    (B) If the mouse is clicked with the SHIFT key down, the user is in
     *        'multi-annotation-selection-mode' and any number of annotation will
     *        be selected when this method completes.
     */
    SelectionItemAnnotation* annotationID = openGLWidget->performIdentificationAnnotations(mouseX,
                                                                                           mouseY);
    Annotation* selectedAnnotation = annotationID->getAnnotation();
    
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager();
    AnnotationManager::SelectionMode selectionMode = AnnotationManager::SELECTION_MODE_SINGLE;
    if (m_allowMultipleSelectionModeFlag) {
        selectionMode = AnnotationManager::SELECTION_MODE_EXTENDED;
    }
    annotationManager->selectAnnotation(selectionMode,
                                        shiftKeyDownFlag,
                                        selectedAnnotation);
    
    setAnnotationUnderMouse(mouseEvent,
                            annotationID);

    const std::vector<Annotation*> allSelectedAnnotations = annotationManager->getSelectedAnnotations();
    if (allSelectedAnnotations.size() == 1) {
        m_annotationBeingEdited = allSelectedAnnotations[0];
    }
    
    if (selectedAnnotation != NULL) {
        m_annotationBeingDragged = selectedAnnotation;
        m_annotationBeingDraggedHandleType = annotationID->getSizingHandle();
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    m_annotationToolsWidget->updateWidget();
}

/**
 * Show a context menu (pop-up menu at mouse location)
 *
 * @param mouseEvent
 *     Mouse event information.
 * @param menuPosition
 *     Point at which menu is displayed (passed to QMenu::exec())
 * @param openGLWidget
 *     OpenGL widget in which context menu is requested
 */
void
UserInputModeAnnotations::showContextMenu(const MouseEvent& mouseEvent,
                                          const QPoint& menuPosition,
                                          BrainOpenGLWidget* openGLWidget)
{
    BrainOpenGLViewportContent* viewportContent = mouseEvent.getViewportContent();
    BrowserTabContent* tabContent = viewportContent->getBrowserTabContent();
    if (tabContent == NULL) {
        return;
    }
    
    const int32_t mouseX = mouseEvent.getX();
    const int32_t mouseY = mouseEvent.getY();
    
    SelectionManager* idManager = openGLWidget->performIdentification(mouseX,
                                                                      mouseY,
                                                                      false);
    
    UserInputModeAnnotationsContextMenu contextMenu(mouseEvent,
                                                    idManager,
                                                    tabContent,
                                                    openGLWidget);
    if (contextMenu.actions().size() > 0) {
        contextMenu.exec(menuPosition);
        
        Annotation* newAnnotation = contextMenu.getNewAnnotationCreatedByContextMenu();
        if (newAnnotation != NULL) {
            selecteAnnotation(newAnnotation);

            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
            EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
        }
    }
}



/* =============================================================================== */

/**
 * Constructor.
 *
 * @param annotationType
 *     Type of annotation being created.
 * @param mouseWindowX
 *     Mouse window X-coordinate
 * @param mouseWindowY
 *     Mouse window Y-coordinate
 * @param windowWidth
 *     Width of window (will not change while user is drawing the annotation)
 * @param windowHeight
 *     Height of window (will not change while user is drawing the annotation)
 */
UserInputModeAnnotations::NewMouseDragCreateAnnotation::NewMouseDragCreateAnnotation(const AnnotationTypeEnum::Enum annotationType,
                                                                                     const MouseEvent& mouseEvent)
{
    BrainOpenGLViewportContent* vpContent = mouseEvent.getViewportContent();
    CaretAssert(vpContent);
    if (vpContent == NULL) {
        CaretLogSevere("Viewport content is invalid.");
        return;
    }
    int32_t windowViewport[4];
    vpContent->getWindowViewport(windowViewport);
    m_windowOriginX = windowViewport[0];
    m_windowOriginY = windowViewport[1];
    
    m_mousePressWindowX = mouseEvent.getX() - m_windowOriginX;
    m_mousePressWindowY = mouseEvent.getY() - m_windowOriginY;
    
    int viewport[4];
    vpContent->getWindowViewport(viewport);
    m_windowWidth  = viewport[2];
    m_windowHeight = viewport[3];
    
    
    
    m_annotation = Annotation::newAnnotationOfType(annotationType,
                                                   AnnotationAttributesDefaultTypeEnum::USER);
    m_annotation->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
    CaretAssert(m_annotation);

    AnnotationOneDimensionalShape* oneDimShape = dynamic_cast<AnnotationOneDimensionalShape*>(m_annotation);
    AnnotationTwoDimensionalShape* twoDimShape = dynamic_cast<AnnotationTwoDimensionalShape*>(m_annotation);
    
    if (oneDimShape != NULL) {
        setCoordinate(oneDimShape->getStartCoordinate(),
                      m_mousePressWindowX,
                      m_mousePressWindowY);
        setCoordinate(oneDimShape->getEndCoordinate(),
                      m_mousePressWindowX,
                      m_mousePressWindowY);
    }
    else if (twoDimShape != NULL) {
        setCoordinate(twoDimShape->getCoordinate(),
                      m_mousePressWindowX,
                      m_mousePressWindowY);
        twoDimShape->setWidth(1.0);
        twoDimShape->setHeight(1.0);
    }
    else {
        CaretAssert(0);
    }
}

/**
 * Destructor.
 */
UserInputModeAnnotations::NewMouseDragCreateAnnotation::~NewMouseDragCreateAnnotation()
{
    delete m_annotation;
}

/**
 * Update with current mouse location.
 *
 * @param mouseWindowX
 *     Mouse window X-coordinate
 * @param mouseWindowY
 *     Mouse window Y-coordinate
 */
void
UserInputModeAnnotations::NewMouseDragCreateAnnotation::update(const int32_t mouseWindowXIn,
                                                               const int32_t mouseWindowYIn)
{
    int32_t mouseWindowX = mouseWindowXIn - m_windowOriginX;
    int32_t mouseWindowY = mouseWindowYIn - m_windowOriginY;
    
    AnnotationOneDimensionalShape* oneDimShape = dynamic_cast<AnnotationOneDimensionalShape*>(m_annotation);
    AnnotationTwoDimensionalShape* twoDimShape = dynamic_cast<AnnotationTwoDimensionalShape*>(m_annotation);
    
    if (oneDimShape != NULL) {
        setCoordinate(oneDimShape->getEndCoordinate(),
                      mouseWindowX,
                      mouseWindowY);
    }
    else if (twoDimShape != NULL) {
        const float minX = std::min(m_mousePressWindowX,
                                    mouseWindowX);
        const float maxX = std::max(m_mousePressWindowX,
                                    mouseWindowX);

        const float minY = std::min(m_mousePressWindowY,
                                    mouseWindowY);
        const float maxY = std::max(m_mousePressWindowY,
                                    mouseWindowY);

        const float x = (minX + maxX) / 2.0;
        const float y = (minY + maxY) / 2.0;
        
        const float width  = maxX - minX;
        const float height = maxY - minY;
        
        const float relativeWidth = 100.0 * ((m_windowWidth > 0.0)
                                             ? (width / static_cast<float>(m_windowWidth))
                                             : 0.01);
        const float relativeHeight = 100.0 * ((m_windowHeight > 0.0)
                                              ? (height / static_cast<float>(m_windowHeight))
                                              : 0.01);
        
        AnnotationCoordinate* coord = twoDimShape->getCoordinate();
        setCoordinate(coord, x, y);
        twoDimShape->setWidth(relativeWidth);
        twoDimShape->setHeight(relativeHeight);
    }
    else {
        CaretAssert(0);
    }
}

/**
 * Set the given coordinate to the XY-location.
 *
 * @param coordinate
 *     Coordinate that is set.
 * @param x
 *     Mouse window X-coordinate
 * @param y
 *     Mouse window Y-coordinate
 */
void
UserInputModeAnnotations::NewMouseDragCreateAnnotation::setCoordinate(AnnotationCoordinate* coordinate,
                                                                      const int32_t x,
                                                                      const int32_t y)
{
    const float relativeX = 100.0 * ((m_windowWidth > 0.0)
                                     ? (x / m_windowWidth)
                                     : 0.01);
    const float relativeY = 100.0 * ((m_windowHeight > 0.0)
                                     ? (y / m_windowHeight)
                                     : 0.01);
    
    coordinate->setXYZ(relativeX,
                       relativeY,
                       0.0);
}

/**
 * @return New annotation being drawn by the user.
 */
const Annotation*
UserInputModeAnnotations::NewMouseDragCreateAnnotation::getAnnotation() const
{
    return m_annotation;
}

