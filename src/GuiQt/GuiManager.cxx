
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

#include <QApplication>

#define __GUI_MANAGER_DEFINE__
#include "GuiManager.h"
#undef __GUI_MANAGER_DEFINE__

#include "BrainBrowserWindow.h"
#include "BrainOpenGL.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserWindowNew.h"
#include "EventManager.h"
#include "SessionManager.h"

#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

#include "CaretAssert.h"

using namespace caret;

/**
 * Constructor.
 * @param parent
 *   Parent of this object.
 */
GuiManager::GuiManager(QObject* parent)
: QObject(parent)
{
    this->nameOfApplication = "Connectome Workbench";
    this->brainOpenGL = NULL;
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_NEW);
}

/**
 * Destructor.
 */
GuiManager::~GuiManager()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    if (this->brainOpenGL != NULL) {
        delete this->brainOpenGL;
        this->brainOpenGL = NULL;
    }
    
}

/**
 * Get the GUI Manager.
 */
GuiManager* 
GuiManager::get()
{
    if (GuiManager::singletonGuiManager == NULL) {
        GuiManager::singletonGuiManager = new GuiManager();
        WuQtUtilities::sendListOfResourcesToCaretLogger();
    }
    return GuiManager::singletonGuiManager;
}

void 
GuiManager::createGuiManager()
{
    CaretAssertMessage((GuiManager::singletonGuiManager == NULL), 
                       "GUI manager has already been created.");
    
    GuiManager::singletonGuiManager = new GuiManager();
}

void 
GuiManager::deleteGuiManager()
{
    CaretAssertMessage((GuiManager::singletonGuiManager != NULL), 
                       "GUI manager does not exist, cannot delete it.");
    
    delete GuiManager::singletonGuiManager;
    GuiManager::singletonGuiManager = NULL;
}

Brain* 
GuiManager::getBrain()
{
    return SessionManager::get()->getBrain(0);
}

/**
 * Get the Brain OpenGL for drawing with OpenGL.
 *
 * @return 
 *    Point to the brain.
 */
BrainOpenGL* 
GuiManager::getBrainOpenGL()
{
    if (this->brainOpenGL == NULL) {
        this->brainOpenGL = BrainOpenGL::getBrainOpenGL();
    }
    
    return this->brainOpenGL;
}

/**
 * See if a brain browser window can be closed.  If there is only
 * one brain browser window, the user will be warned that any 
 * changes to files will be lost and the application will exit.
 * If there is more than one brain browser window open and the 
 * window being closed contains more than one tab, the user will
 * be warned.
 *
 * @param brainBrowserWindow
 *   Brain browser window that will be closed.
 * @param numberOfOpenTabs
 *   Number of tabs in window.
 * @return 
 *   True if window should be closed, else false.
 */
bool 
GuiManager::allowBrainBrowserWindowToClose(BrainBrowserWindow* brainBrowserWindow,
                                           const int32_t numberOfOpenTabs)
{
    bool isBrowserWindowAllowedToClose = false;
    
    if (this->getNumberOfBrainBrowserWindows() > 1) {
        /*
         * Warn if multiple tabs in window
         */
        if (numberOfOpenTabs > 1) {
            QString tabMessage = QString::number(numberOfOpenTabs) + " tabs are open.";
            isBrowserWindowAllowedToClose =
                WuQMessageBox::warningCloseCancel(brainBrowserWindow, 
                                                  "Are you sure you want to close this window?", 
                                                  tabMessage);
        }
        else {
            isBrowserWindowAllowedToClose = true;
        }
    }
    else {
        isBrowserWindowAllowedToClose = this->exitProgram(brainBrowserWindow);
    }
    
    if (isBrowserWindowAllowedToClose) {
        for (int32_t i = 0; i < static_cast<int32_t>(this->brainBrowserWindows.size()); i++) {
            if (this->brainBrowserWindows[i] == brainBrowserWindow) {
                this->brainBrowserWindows[i] = NULL;
            }
        }
    }
    
    return isBrowserWindowAllowedToClose;
}

/**
 * Get the number of brain browser windows.
 *
 * @return Number of brain browser windows that are valid.
 */
int32_t 
GuiManager::getNumberOfBrainBrowserWindows() const
{
    int32_t numberOfWindows = 0;
    for (int32_t i = 0; i < static_cast<int32_t>(this->brainBrowserWindows.size()); i++) {
        if (this->brainBrowserWindows[i] != NULL) {
            numberOfWindows++;
        }
    }
    return numberOfWindows;
}

/**
 * Get all of the brain browser windows.
 *
 * @return 
 *   Vector containing all open brain browser windows.
 */
QVector<BrainBrowserWindow*> 
GuiManager::getAllBrainBrowserWindows() const
{ 
    QVector<BrainBrowserWindow*> windows;
    
    int32_t numWindows = static_cast<int32_t>(this->brainBrowserWindows.size());
    for (int32_t i = 0; i < numWindows; i++) {
        if (this->brainBrowserWindows[i] != NULL) {
            windows.push_back(this->brainBrowserWindows[i]);
        }
    }
    
    return windows; 
}

/**
 * Create a new BrainBrowser Window.
 * @param parent
 *    Optional parent that is used only for window placement.
 * @param browserTabContent
 *    Optional tab for initial windwo tab.
 */
BrainBrowserWindow*
GuiManager::newBrainBrowserWindow(QWidget* parent,
                                  BrowserTabContent* browserTabContent)
{
    int32_t windowIndex = -1;
    
    int32_t numWindows = static_cast<int32_t>(this->brainBrowserWindows.size());
    for (int32_t i = 0; i < numWindows; i++) {
        if (this->brainBrowserWindows[i] == NULL) {
            windowIndex = i;
            break;
        }
    }
    
    BrainBrowserWindow* bbw = NULL; 
    
    if (windowIndex < 0) {
        windowIndex = this->brainBrowserWindows.size();
        bbw = new BrainBrowserWindow(windowIndex, browserTabContent);
        this->brainBrowserWindows.push_back(bbw);
    }
    else {
        bbw = new BrainBrowserWindow(windowIndex, browserTabContent);
        this->brainBrowserWindows[windowIndex] = bbw;
    }
    
    if (parent != NULL) {
        WuQtUtilities::moveWindowToOffset(parent, bbw, 20, 20);
    }
    
    bbw->show(); 
    
    return bbw;
}

/**
 * Exit the program.
 * @param
 *    Parent over which dialogs are displayed for saving/verifying.
 * return 
 *    true if application should exit, else false.
 */
bool 
GuiManager::exitProgram(QWidget* parent)
{
    bool okToExit = false;
    
    /*
     * Are files modified?
     */
    const bool areFilesModified = false;
    if (areFilesModified) {
        WuQMessageBox::StandardButton buttonPressed = 
        WuQMessageBox::saveDiscardCancel(parent, 
                                         "Files are modified.", 
                                         "Do you want to save changes?");
        
        switch (buttonPressed) {
            case QMessageBox::Save:
                break;
            case QMessageBox::Discard:
                okToExit = true;
                break;
            case QMessageBox::Cancel:
                break;
            default:
                CaretAssert(0);
                break;
        }
        
    }
    else {
        okToExit =
        WuQMessageBox::warningOkCancel(parent,
                                       "<html>Closing this window will<br>exit the application.</html>");
    }
    
    if (okToExit) {
        QVector<BrainBrowserWindow*> bws = this->getAllBrainBrowserWindows();
        for (int i = 0; i < bws.size(); i++) {
            bws[i]->deleteLater();
        }
        
        QCoreApplication::instance()->quit();
    }    
    
    return okToExit;
}

/**
 * Get the browser tab content in a browser window.
 * @param browserWindowIndex
 *    Index of browser window.
 * @return
 *    Browser tab content in the browser window.
 */
BrowserTabContent* 
GuiManager::getBrowserTabContentForBrowserWindow(const int32_t browserWindowIndex)
{
    CaretAssertVectorIndex(this->brainBrowserWindows, browserWindowIndex);
    BrainBrowserWindow* browserWindow = brainBrowserWindows[browserWindowIndex];
    CaretAssert(browserWindow);
    
    BrowserTabContent* tabContent = browserWindow->getBrowserTabContent();
    CaretAssert(tabContent);
    return tabContent;
}

/**
 * Called when bring all windows to front is selected.
 */
void 
GuiManager::processBringAllWindowsToFront()
{
    for (int32_t i = 0; i < static_cast<int32_t>(this->brainBrowserWindows.size()); i++) {
        this->brainBrowserWindows[i]->show();
        this->brainBrowserWindows[i]->activateWindow();
    }
}

/**
 * Called when show identify window is selected.
 */
void 
GuiManager::processShowIdentifyWindow()
{
    
}

/**
 * Called when show data display window is selected.
 */
void 
GuiManager::processShowDataDisplayWindow()
{
    
}

/**
 * Called when show help online is selected.
 */ 
void 
GuiManager::processShowHelpOnlineWindow()
{
    
}

/**
 * Called when search help online is selected.
 */
void 
GuiManager::processShowSearchHelpOnlineWindow()
{
    
}

QString 
GuiManager::applicationName() const
{
    return this->nameOfApplication;
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
GuiManager::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_NEW) {
        EventBrowserWindowNew* eventNewBrowser =
            dynamic_cast<EventBrowserWindowNew*>(event);
        CaretAssert(eventNewBrowser);
        
        BrainBrowserWindow* bbw = this->newBrainBrowserWindow(eventNewBrowser->getParent(), 
                                                              eventNewBrowser->getBrowserTabContent());
        eventNewBrowser->setBrowserWindowCreated(bbw);
        
        eventNewBrowser->setEventProcessed();
    }
    else {
        
    }
}




