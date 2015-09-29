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

#ifndef CENTRYDISPLAY_H
#define CENTRYDISPLAY_H

#include <QString>

// Sword includes:
#include <swdisp.h>
#include <swmodule.h>

#include "backend/drivers/btmodulelist.h"
#include "ctextrendering.h"


struct DisplayOptions;
struct FilterOptions;

namespace Rendering {

class CEntryDisplay: public sword::SWDisplay {

    public: /* Methods: */

        /**
          \returns the rendered text using the modules in the list and using the
                   key parameter.
        */
        virtual const QString text(const BtConstModuleList &modules,
                                   const QString &key,
                                   const DisplayOptions &displayOptions,
                                   const FilterOptions &filterOptions);

    const QString textKeyRendering(const BtConstModuleList &modules,
                               const QString &key,
                               const DisplayOptions &displayOptions,
                               const FilterOptions &filterOptions,
                               CTextRendering::KeyTreeItem::Settings::KeyRenderingFace keyRendering,
                               bool introdutions = true);

        virtual char display(sword::SWModule& mod) { (void)mod; return 'c';}

}; /* class CEntryDisplay */

} /* namespace Rendering */

#endif
