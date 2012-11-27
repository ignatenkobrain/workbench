
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#define __IDENTIFICATION_ITEM_VOXEL_DECLARE__
#include "SelectionItemVoxel.h"
#undef __IDENTIFICATION_ITEM_VOXEL_DECLARE__

#include "CaretAssert.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * \class SelectionItemVoxel
 * \brief Selected voxel.
 *
 * Information about an selected voxel.
 */


/**
 * Constructor.
 */
SelectionItemVoxel::SelectionItemVoxel()
: SelectionItem(SelectionItemDataTypeEnum::VOXEL)
{
    m_volumeFile = NULL;
    m_voxelIJK[0] = -1;
    m_voxelIJK[1] = -1;
    m_voxelIJK[2] = -1;
}

/**
 * Destructor.
 */
SelectionItemVoxel::~SelectionItemVoxel()
{
    
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemVoxel::SelectionItemVoxel(const SelectionItemVoxel& obj)
: SelectionItem(obj)
{
    copyHelperSelectionItemVoxel(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to m_.
 * @return
 *    Reference to m_ object.
 */
SelectionItemVoxel&
SelectionItemVoxel::operator=(const SelectionItemVoxel& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        copyHelperSelectionItemVoxel(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of m_ type.
 * @param ff
 *    Object that is copied.
 */
void
SelectionItemVoxel::copyHelperSelectionItemVoxel(const SelectionItemVoxel& idItem)
{
    m_volumeFile  = idItem.m_volumeFile;
    m_voxelIJK[0] = idItem.m_voxelIJK[0];
    m_voxelIJK[1] = idItem.m_voxelIJK[1];
    m_voxelIJK[2] = idItem.m_voxelIJK[2];
}

/**
 * Reset this selection item. 
 */
void 
SelectionItemVoxel::reset()
{
    SelectionItem::reset();
    m_volumeFile = NULL;
    m_voxelIJK[0] = -1;
    m_voxelIJK[1] = -1;
    m_voxelIJK[2] = -1;
}

/**
 * @return The volume file.
 */
const VolumeFile* 
SelectionItemVoxel::getVolumeFile() const
{
    return m_volumeFile;
}

/**
 * Get the voxel indices.
 * @param voxelIJK
 *    Output containing voxel indices.
 */
void 
SelectionItemVoxel::getVoxelIJK(int64_t voxelIJK[3]) const
{
    voxelIJK[0] = m_voxelIJK[0];
    voxelIJK[1] = m_voxelIJK[1];
    voxelIJK[2] = m_voxelIJK[2];
}

/**
 * Set the volume file.
 * @param volumeFile
 *    New value for volume file.
 */
void 
SelectionItemVoxel::setVolumeFile(VolumeFile* volumeFile)
{
    m_volumeFile = volumeFile;
}

/**
 * Set the voxel indices.
 * @param voxelIJK
 *    New value for voxel indices.
 */
void 
SelectionItemVoxel::setVoxelIJK(const int64_t voxelIJK[3])
{
    m_voxelIJK[0] = voxelIJK[0];
    m_voxelIJK[1] = voxelIJK[1];
    m_voxelIJK[2] = voxelIJK[2];
}

/**
 * @return Is this selected item valid?
 */
bool 
SelectionItemVoxel::isValid() const
{
    return (m_volumeFile != NULL);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SelectionItemVoxel::toString() const
{
    AString text = SelectionItem::toString();
    text += ("Volume: " + ((m_volumeFile != NULL) ? m_volumeFile->getFileNameNoPath() : "INVALID"));
    text += ("Voxel: " 
             + AString::number(m_voxelIJK[0]) + ", "
             + AString::number(m_voxelIJK[1]) + ", "
             + AString::number(m_voxelIJK[2]) + "\n");
    
    return text;
}



