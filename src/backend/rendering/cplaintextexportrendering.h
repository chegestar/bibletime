/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef RENDERINGCPLAINTEXTEXPORTRENDERING_H
#define RENDERINGCPLAINTEXTEXPORTRENDERING_H

#include "backend/rendering/chtmlexportrendering.h"
#include "backend/config/btconfig.h"


namespace Rendering {

/**
 * This implementation can be used to export content as plain text.
 * @short Text rendering as plain text.
 * @author The BibleTime team
 */
class CPlainTextExportRendering: public CHTMLExportRendering {

    public: /* Methods: */

        CPlainTextExportRendering(
            bool addText,
            const DisplayOptions &displayOptions = btConfig().getDisplayOptions(),
            const FilterOptions &filterOptions = btConfig().getFilterOptions());

    protected: /* Methods: */

        virtual QString renderEntry(const KeyTreeItem &item, CSwordKey * key = 0);
        virtual QString finishText(const QString &text, const KeyTree &tree);

}; /* class CPlainTextExportRendering */

} /* namespace Rendering */

#endif
