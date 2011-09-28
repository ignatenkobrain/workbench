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

#include <algorithm>

#include "EventManager.h"
#include "EventModelDisplayControllerGetAll.h"
#include "ModelDisplayControllerSurface.h"
#include "ModelDisplayControllerWholeBrain.h"
#include "Surface.h"

using namespace caret;

/**
 * Constructor.
 * @param brain - brain to which this surface controller belongs.
 * @param surface - surface for this controller.
 *
 */
ModelDisplayControllerWholeBrain::ModelDisplayControllerWholeBrain(Brain* brain)
: ModelDisplayController(ModelDisplayControllerTypeEnum::MODEL_TYPE_WHOLE_BRAIN,
                         YOKING_ALLOWED_NO,
                         ROTATION_ALLOWED_YES)
{
    this->initializeMembersModelDisplayControllerWholeBrain();
}

/**
 * Destructor
 */
ModelDisplayControllerWholeBrain::~ModelDisplayControllerWholeBrain()
{
}

void
ModelDisplayControllerWholeBrain::initializeMembersModelDisplayControllerWholeBrain()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->selectedSurfaceType[i] = SurfaceTypeEnum::SURFACE_TYPE_ANATOMICAL;
    }
}

/**
 * Get the brain that created this controller.
 * @return The brain.
 */
Brain*
ModelDisplayControllerWholeBrain::getBrain()
{
    return this->brain;
}

void 
ModelDisplayControllerWholeBrain::getAvailableSurfaceTypes(std::vector<SurfaceTypeEnum::Enum> surfaceTypesOut)
{
    this->updateController();
    
    surfaceTypesOut.clear();
    surfaceTypesOut.insert(surfaceTypesOut.end(),
                           this->availableSurfaceTypes.begin(),
                           this->availableSurfaceTypes.end());
}

SurfaceTypeEnum::Enum 
ModelDisplayControllerWholeBrain::getSelectedSurfaceType(const int32_t windowTabNumber)
{
    this->updateController();
    return this->selectedSurfaceType[windowTabNumber];    
}

void 
ModelDisplayControllerWholeBrain::updateController()
{
    /*
     * Get all model controllers to find loaded surface types.
     */
    EventModelDisplayControllerGetAll eventGetControllers;
    EventManager::get()->sendEvent(eventGetControllers.getPointer());
    const std::vector<ModelDisplayController*> allControllers =
        eventGetControllers.getModelDisplayControllers();

    /*
     * Get ALL possible surface types.
     */
    std::vector<SurfaceTypeEnum::Enum> allSurfaceTypes;
    SurfaceTypeEnum::getAllEnums(allSurfaceTypes);
    const int32_t numEnumTypes = static_cast<int32_t>(allSurfaceTypes.size());
    std::vector<bool> surfaceTypeValid(numEnumTypes, false);
    
    /*
     * Find surface types that are actually used.
     */
    for (std::vector<ModelDisplayController*>::const_iterator iter = allControllers.begin();
         iter != allControllers.end();
         iter++) {
        ModelDisplayControllerSurface* surfaceController = 
            dynamic_cast<ModelDisplayControllerSurface*>(*iter);
        if (surfaceController != NULL) {
            SurfaceTypeEnum::Enum surfaceType = surfaceController->getSurface()->getSurfaceType();
            
            for (int i = 0; i < numEnumTypes; i++) {
                if (allSurfaceTypes[i] == surfaceType) {
                    surfaceTypeValid[i] = true;
                    break;
                }
            }
        }
    }
    
    /*
     * Set the available surface types.
     */
    this->availableSurfaceTypes.clear();
    for (int i = 0; i < numEnumTypes; i++) {
        if (surfaceTypeValid[i]) {
            this->availableSurfaceTypes.push_back(allSurfaceTypes[i]);
            break;
        }
    }
    
    /*
     * Update the selected surface type.
     */
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        if (std::find(this->availableSurfaceTypes.begin(),
                      this->availableSurfaceTypes.end(),
                      this->selectedSurfaceType[i]) == availableSurfaceTypes.end()) {
            if (this->availableSurfaceTypes.empty() == false) {
                this->selectedSurfaceType[i] = this->availableSurfaceTypes[0];
            }
            else {
                this->selectedSurfaceType[i] = SurfaceTypeEnum::SURFACE_TYPE_ANATOMICAL;
            }
        }
    }
}

void 
ModelDisplayControllerWholeBrain::setSelectedSurfaceType(const int32_t windowTabNumber,
                                                         const SurfaceTypeEnum::Enum surfaceType)
{
    this->selectedSurfaceType[windowTabNumber] = surfaceType;
    this->updateController();
}

/**
 * Get the name for use in a GUI.
 *
 * @param includeStructureFlag - Prefix label with structure to which
 *      this structure model belongs.
 * @return   Name for use in a GUI.
 *
 */
AString
ModelDisplayControllerWholeBrain::getNameForGUI(const bool includeStructureFlag) const
{
    return "ModelDisplayControllerWholeBrain::getNameForGUI_NEEDS_IMPLEMENTATION";
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model controller.
 */
AString 
ModelDisplayControllerWholeBrain::getNameForBrowserTab() const
{
    return "Whole Brain";
}

