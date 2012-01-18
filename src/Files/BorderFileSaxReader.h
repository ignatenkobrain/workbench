
#ifndef __BORDER_FILE_SAX_READER_H__
#define __BORDER_FILE_SAX_READER_H__

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
/*LICENSE_END*/

#include <stack>
#include <AString.h>
#include <stdint.h>

#include "XmlSaxParserException.h"
#include "XmlSaxParserHandlerInterface.h"


namespace caret {

    class Border;
    class BorderFile;
    class GiftiMetaDataSaxReader;
    class SurfaceProjectedItem;
    class SurfaceProjectedItemSaxReader;
    class XmlAttributes;
    class XmlException;
    
    class BorderFileSaxReader : public CaretObject, public XmlSaxParserHandlerInterface {
    public:
        BorderFileSaxReader(BorderFile* borderFile);
        
        virtual ~BorderFileSaxReader();
        
        void startElement(const AString& namespaceURI,
                          const AString& localName,
                          const AString& qName,
                          const XmlAttributes& attributes) throw (XmlSaxParserException);
        
        void endElement(const AString& namspaceURI,
                        const AString& localName,
                        const AString& qName) throw (XmlSaxParserException);
        
        void characters(const char* ch) throw (XmlSaxParserException);
        
        void fatalError(const XmlSaxParserException& e) throw (XmlSaxParserException);
        
        void warning(const XmlSaxParserException& e) throw (XmlSaxParserException);
        
        void error(const XmlSaxParserException& e) throw (XmlSaxParserException);
        
        void startDocument() throw (XmlSaxParserException);
        
        void endDocument() throw (XmlSaxParserException);
        
        
    protected:
        /// file reading states
        enum STATE {
            /// no state
            STATE_NONE,
            /// processing BorderFile tag
            STATE_BORDER_FILE,
            /// processing MetaData tag
            STATE_METADATA,
            /// processing Border
            STATE_BORDER,
            /// processing SurfaceProjectedItem tag
            STATE_SURFACE_PROJECTED_ITEM
        };
        
        /// file reading state
        STATE state;
        
        /// the state stack used when reading a file
        std::stack<STATE> stateStack;
        
        /// the error message
        AString errorMessage;
        
        /// Border file that is being read
        BorderFile* borderFile;
        
        /// border that is being read
        Border* border;
        
        /// surface projected item that is being read
        SurfaceProjectedItem* surfaceProjectedItem;
        
        /// Reads a SurfaceProjectedItem
        SurfaceProjectedItemSaxReader* surfaceProjectedItemSaxReader;
        
        /// element text
        AString elementText;
        
        /// GIFTI meta data sax reader
        GiftiMetaDataSaxReader* metaDataSaxReader;        
    };

} // namespace

#endif // __BORDER_FILE_SAX_READER_H__

