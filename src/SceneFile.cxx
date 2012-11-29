
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

#include <QTextStream>

#include <algorithm>
#include <memory>

#define __SCENE_FILE_DECLARE__
#include "SceneFile.h"
#undef __SCENE_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "FileAdapter.h"
#include "FileInformation.h"
#include "GiftiMetaData.h"
#include "Scene.h"
#include "SceneFileSaxReader.h"
#include "SceneWriterXml.h"
#include "XmlSaxParser.h"
#include "XmlWriter.h"

using namespace caret;


    
/**
 * \class caret::SceneFile 
 * \brief Contains scenes that reproduce Workbench state
 */

/**
 * Constructor.
 */
SceneFile::SceneFile()
: CaretDataFile(DataFileTypeEnum::SCENE)
{
    m_metadata = new GiftiMetaData();
}

/**
 * Destructor.
 */
SceneFile::~SceneFile()
{
    delete m_metadata;
    
    for (std::vector<Scene*>::iterator iter = m_scenes.begin();
         iter != m_scenes.end();
         iter++) {
        delete *iter;
    }
    m_scenes.clear();
}

/**
 * Clear the contents of this file.
 */
void 
SceneFile::clear()
{
    CaretDataFile::clear();
    
    m_metadata->clear();
    
    for (std::vector<Scene*>::iterator iter = m_scenes.begin();
         iter != m_scenes.end();
         iter++) {
        delete *iter;
    }
    m_scenes.clear();
}

/**
 * @return true if the file is empty (no scenes) else false.
 */
bool 
SceneFile::isEmpty() const
{
    return m_scenes.empty();
}


/**
 * Add the given scene to the file.  The file then
 * takes ownership of the scene.
 * 
 * @param scene
 *    Scene that is added.
 */
void 
SceneFile::addScene(Scene* scene)
{
    CaretAssert(scene);
    m_scenes.push_back(scene);
    setModified();
}

/** 
 * Replace a scene.
 * @param newScene
 *    New scene
 * @param sceneThatIsReplacedAndDeleted
 *    Scene that is replaced and delete so DO NOT
 *    reference this scene after calling this method.
 */
void
SceneFile::replaceScene(Scene* newScene,
                        Scene* sceneThatIsReplacedAndDeleted)
{
    CaretAssert(newScene);
    CaretAssert(sceneThatIsReplacedAndDeleted);
    
    const int32_t numScenes = getNumberOfScenes();
    for (int32_t i = 0; i < numScenes; i++) {
        if (m_scenes[i] == sceneThatIsReplacedAndDeleted) {
            delete m_scenes[i];
            m_scenes[i] = newScene;
            setModified();
            return;
        }
    }
    
    CaretAssertMessage(0, "Replacing scene failed due to scene not found.");
    CaretLogSevere("Replacing scene failed due to scene not found.");
}

/**
 * @return The number of scenes.
 */
int32_t 
SceneFile::getNumberOfScenes() const
{
    return m_scenes.size();
}

/**
 * Get the scene at the given index.
 * @param indx
 *     Index of the scene.
 * @return
 *     Scene at the given index.
 */
Scene* 
SceneFile::getSceneAtIndex(const int32_t indx)
{
    CaretAssertVectorIndex(m_scenes, indx);
    return m_scenes[indx];
}

/**
 * Get the scene with the given name.
 * @param sceneName
 *    Name of scene.
 * @return 
 *    Scene with given name or NULL if no scene with
 *    the given name.
 */
Scene*
SceneFile::getSceneWithName(const AString& sceneName)
{
    const int32_t numScenes = getNumberOfScenes();
    for (int32_t i = 0; i < numScenes; i++) {
        Scene* scene = getSceneAtIndex(i);
        if (scene->getName() == sceneName) {
            return scene;
        }
    }
    return NULL;
}


/**
 * Remove the given scene.
 * @param scene
 *    Scene that should be removed.
 */
void 
SceneFile::removeScene(Scene* scene)
{
    CaretAssert(scene);
    std::vector<Scene*>::iterator iter = std::find(m_scenes.begin(),
                                                   m_scenes.end(),
                                                   scene);
    if (iter != m_scenes.end()) {
        m_scenes.erase(iter);
        delete scene;
        setModified();
    }
}


/**
 * Remove the scene at the given index.
 * @param indx
 *     Index of the scene.
 */
void 
SceneFile::removeSceneAtIndex(const int32_t indx)
{
    CaretAssertVectorIndex(m_scenes, indx);
    Scene* scene = getSceneAtIndex(indx);
    removeScene(scene);
}

/**
 * Reorder the scenes given the newly ordered scenes.
 * Any existing scenes not in the newly ordered scenes are 
 * removed.
 *
 * @param orderedScenes
 *    Newly ordered scenes.
 */
void 
SceneFile::reorderScenes(std::vector<Scene*>& orderedScenes)
{
    /*
     * Make copy of pointers to existing scenes
     */
    std::vector<Scene*> oldSceneVector = m_scenes;
    
    /*
     * Replace scenes with newly ordered scenes
     */
    m_scenes = orderedScenes;
    
    /*
     * If an existing scene is not in the newly ordered scenes,
     * remove it.
     */
    for (std::vector<Scene*>::iterator iter = oldSceneVector.begin();
         iter != oldSceneVector.end();
         iter++) {
        Scene* scene = *iter;
        if (std::find(m_scenes.begin(),
                      m_scenes.end(),
                      scene) == m_scenes.end()) {
            delete scene;
        }
    }
    
    setModified();
}


/**
 * @return The structure for this file.
 */
StructureEnum::Enum 
SceneFile::getStructure() const
{
    return StructureEnum::ALL;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void 
SceneFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    /* ignore, not a structure related file */
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData* 
SceneFile::getFileMetaData()
{
    return m_metadata;
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData* 
SceneFile::getFileMetaData() const
{
    return m_metadata;
}

/**
 * Read the scene file.
 * @param filename
 *    Name of scene file.
 * @throws DataFileException
 *    If there is an error reading the file.
 */
void 
SceneFile::readFile(const AString& filenameIn) throw (DataFileException)
{
    FileInformation fileInfo(filenameIn);
    const AString filename = fileInfo.getFilePath();
    this->setFileName(filename);
    SceneFileSaxReader saxReader(this);
    std::auto_ptr<XmlSaxParser> parser(XmlSaxParser::createXmlParser());
    try {
        parser->parseFile(filename, &saxReader);
    }
    catch (const XmlSaxParserException& e) {
        this->setFileName("");
        
        int lineNum = e.getLineNumber();
        int colNum  = e.getColumnNumber();
        
        AString msg =
        "Parse Error while reading "
        + filename;
        
        if ((lineNum >= 0) && (colNum >= 0)) {
            msg += (" line/col ("
                    + AString::number(e.getLineNumber())
                    + "/"
                    + AString::number(e.getColumnNumber())
                    + ")");
        }
        
        msg += (": " + e.whatString());
        
        DataFileException dfe(msg);
        CaretLogThrowing(dfe);
        throw dfe;
    }
    
    this->setFileName(filename);

    this->clearModified();
}

/**
 * Write the scene file.
 * @param filename
 *    Name of scene file.
 * @throws DataFileException
 *    If there is an error writing the file.
 */
void 
SceneFile::writeFile(const AString& filename) throw (DataFileException)
{
    this->setFileName(filename);
    
    try {
        //
        // Format the version string so that it ends with at most one zero
        //
        const AString versionString = AString::number(1.0);
        
        //
        // Open the file
        //
        FileAdapter file;
        AString errorMessage;
        QTextStream* textStream = file.openQTextStreamForWritingFile(this->getFileName(),
                                                                     errorMessage);
        if (textStream == NULL) {
            throw DataFileException(errorMessage);
        }
        
        //
        // Create the xml writer
        //
        XmlWriter xmlWriter(*textStream);
        
        //
        // Write header info
        //
        xmlWriter.writeStartDocument("1.0");
        
        //
        // Write root element
        //
        XmlAttributes attributes;
        
        //attributes.addAttribute("xmlns:xsi",
        //                        "http://www.w3.org/2001/XMLSchema-instance");
        //attributes.addAttribute("xsi:noNamespaceSchemaLocation",
        //                        "http://brainvis.wustl.edu/caret6/xml_schemas/GIFTI_Caret.xsd");
        attributes.addAttribute(SceneFile::XML_ATTRIBUTE_VERSION,
                                versionString);
        xmlWriter.writeStartElement(SceneFile::XML_TAG_SCENE_FILE,
                                    attributes);
        
        //
        // Write Metadata
        //
        if (m_metadata != NULL) {
            m_metadata->writeAsXML(xmlWriter);
        }
        
        //
        // Write scenes
        //
        SceneWriterXml sceneWriter(xmlWriter,
                                   this->getFileName());
        const int32_t numScenes = this->getNumberOfScenes();
        for (int32_t i = 0; i < numScenes; i++) {
            sceneWriter.writeScene(*m_scenes[i], 
                                   i);
        }
        
        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
        
        file.close();
        
        this->clearModified();
    }
    catch (const GiftiException& e) {
        throw DataFileException(e);
    }
    catch (const XmlException& e) {
        throw DataFileException(e);
    }
}


