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

#ifndef CHTMLWRITEWINDOW_H
#define CHTMLWRITEWINDOW_H

#include "frontend/displaywindow/cplainwritewindow.h"


/**
 * The write window class which offers a WYSIWYG text editor for creating a personal commentary.
  *
  * Inherits CPlainWriteWindow.
  *
  * @author The BibleTime team
  */
class CHTMLWriteWindow : public CPlainWriteWindow  {
        Q_OBJECT
    public:
        CHTMLWriteWindow(const QList<CSwordModuleInfo *> & modules, CMDIArea * parent);

        virtual void storeProfileSettings(const QString & windowGroup);
        virtual void applyProfileSettings(const QString & windowGroup);

        /**
         * Returns true if the sync toolbar is enabled.
         */
        virtual bool syncAllowed() const;

    protected:
        /**
        * Initialize the state of this widget.
        */
        virtual void initView();
        virtual void initConnections();
        virtual void initToolbars();

        virtual WriteWindowType writeWindowType() const {
            return HTMLWindow;
        }
        /**
         * Called to add actions to mainWindow toolbars
         */
        virtual void setupMainWindowToolBars();

    protected slots:
        /**
        * Is called when the current text was changed.
        */
        virtual void textChanged();
        /**
        * Loads the original text from the module.
        */
        virtual void restoreText();
        /**
        * Saves the text for the current key. Directly writes the changed text into the module.
        */
        virtual void saveCurrentText( const QString& );

};

#endif
