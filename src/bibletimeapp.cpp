/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bibletimeapp.h"

#include <QDebug>
#include <QPainter>
#include <QtGlobal>
#include "frontend/messagedialog.h"
#include "backend/config/btconfig.h"
#include "backend/managers/cswordbackend.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "util/bticons.h"


BibleTimeApp::BibleTimeApp(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_init(false)
    , m_icons(NULL)
{
    setApplicationName("bibletime");
    setApplicationVersion(BT_VERSION);

    // Support for retina displays
    #if QT_VERSION >= 0x050200
    this->setAttribute(Qt::AA_UseHighDpiPixmaps);
    #endif
}

BibleTimeApp::~BibleTimeApp() {
    // Prevent writing to the log file before the directory cache is init:
    if (!m_init || BtConfig::m_instance == 0)
        return;

    //we can set this safely now because we close now (hopyfully without crash)
    btConfig().setValue("state/crashedLastTime", false);
    btConfig().setValue("state/crashedTwoTimes", false);

    delete CDisplayTemplateMgr::instance();
    CLanguageMgr::destroyInstance();
    CSwordBackend::destroyInstance();
    delete m_icons;

    BtConfig::destroyInstance();
}

bool BibleTimeApp::initBtConfig() {
    Q_ASSERT(m_init);

    BtConfig::InitState const r = BtConfig::initBtConfig();
    if (r == BtConfig::INIT_OK)
        return true;
#ifndef BT_MINI
    if (r == BtConfig::INIT_NEED_UNIMPLEMENTED_FORWARD_MIGRATE) {
        /// \todo Migrate from btConfigOldApi to BTCONFIG_API_VERSION
        qWarning() << "BibleTime configuration migration is not yet implemented!!!";
        if (message::showWarning(
                    NULL,
                    tr("Warning!"),
                    tr("Migration to the new configuration system is not yet "
                       "implemented. Proceeding might result in <b>loss of data"
                       "</b>. Please backup your configuration files before "
                       "you continue!<br/><br/>Do you want to continue? Press "
                       "\"No\" to quit BibleTime immediately."),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No) == QMessageBox::No)
            return false;
    } else {
        Q_ASSERT(r == BtConfig::INIT_NEED_UNIMPLEMENTED_BACKWARD_MIGRATE);
        if (message::showWarning(
                    NULL,
                    tr("Error loading configuration!"),
                    tr("Failed to load BibleTime's configuration, because it "
                       "appears that the configuration file corresponds to a "
                       "newer version of BibleTime. This is likely caused by "
                       "BibleTime being downgraded. Loading the new "
                       "configuration file may result in <b>loss of data</b>."
                       "<br/><br/>Do you still want to try to load the new "
                       "configuration file? Press \"No\" to quit BibleTime "
                       "immediately."),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No) == QMessageBox::No)
            return false;
    }
#endif
    BtConfig::forceMigrate();
    return true;
}

bool BibleTimeApp::initDisplayTemplateManager() {
    Q_ASSERT(m_init);

    QString errorMessage;
    new CDisplayTemplateMgr(errorMessage);
    if (errorMessage.isNull())
        return true;
#ifndef BT_MINI
    message::showCritical(0, tr("Fatal error!"), errorMessage);
#endif
    return false;
}

void BibleTimeApp::initIcons() { m_icons = new BtIcons(); }
