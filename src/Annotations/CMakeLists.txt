#
# Name of Project
#
PROJECT (Annotations)

#
# Use XML from Qt but not GUI.
#
SET(QT_DONT_USE_QTGUI TRUE)

#
# QT include files
#
###INCLUDE(${QT_USE_FILE})

#
# Create a Library
#
ADD_LIBRARY(Annotations
Annotation.h
AnnotationAlignmentEnum.h
AnnotationAttributesDefaultTypeEnum.h
AnnotationBox.h
AnnotationColorBar.h
AnnotationColorBarNumericText.h
AnnotationColorBarPositionModeEnum.h
AnnotationColorBarSection.h
AnnotationCoordinate.h
AnnotationCoordinateSpaceEnum.h
AnnotationTextFontPointSizeEnum.h
AnnotationImage.h
AnnotationLine.h
AnnotationOneDimensionalShape.h
AnnotationOval.h
AnnotationPercentSizeText.h
AnnotationPointSizeText.h
AnnotationSizingHandleTypeEnum.h
AnnotationText.h
AnnotationTextAlignHorizontalEnum.h
AnnotationTextAlignVerticalEnum.h
AnnotationTextConnectTypeEnum.h
AnnotationTextFontNameEnum.h
AnnotationTextFontSizeTypeEnum.h
AnnotationTextOrientationEnum.h
AnnotationTwoDimensionalShape.h
AnnotationTypeEnum.h
AnnotationRedoUndoCommand.h
AnnotationRedoUndoCommandModeEnum.h
EventAnnotationDeleteUndeleteFromFile.h

Annotation.cxx
AnnotationAlignmentEnum.cxx
AnnotationAttributesDefaultTypeEnum.cxx
AnnotationBox.cxx
AnnotationColorBar.cxx
AnnotationColorBarNumericText.cxx
AnnotationColorBarPositionModeEnum.cxx
AnnotationColorBarSection.cxx
AnnotationCoordinate.cxx
AnnotationCoordinateSpaceEnum.cxx
AnnotationTextFontPointSizeEnum.cxx
AnnotationImage.cxx
AnnotationLine.cxx
AnnotationOneDimensionalShape.cxx
AnnotationOval.cxx
AnnotationPercentSizeText.cxx
AnnotationPointSizeText.cxx
AnnotationSizingHandleTypeEnum.cxx
AnnotationText.cxx
AnnotationTextAlignHorizontalEnum.cxx
AnnotationTextAlignVerticalEnum.cxx
AnnotationTextConnectTypeEnum.cxx
AnnotationTextFontNameEnum.cxx
AnnotationTextFontSizeTypeEnum.cxx
AnnotationTextOrientationEnum.cxx
AnnotationTwoDimensionalShape.cxx
AnnotationTypeEnum.cxx
AnnotationRedoUndoCommand.cxx
AnnotationRedoUndoCommandModeEnum.cxx
EventAnnotationDeleteUndeleteFromFile.cxx
)

#
# Find headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Scenes
${CMAKE_SOURCE_DIR}/Nifti
)

