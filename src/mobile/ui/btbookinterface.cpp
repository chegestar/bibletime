
#include "btbookinterface.h"

#include "backend/config/btconfig.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/drivers/cswordbookmoduleinfo.h"
#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordtreekey.h"
#include "backend/managers/cswordbackend.h"
#include "backend/rendering/centrydisplay.h"
#include "backend/rendering/cdisplayrendering.h"
#include "mobile/btmmain.h"
#include "mobile/keychooser/bookkeychooser.h"
#include "mobile/ui/bookmodulechooser.h"
#include "mobile/ui/viewmanager.h"
#include <QDebug>
#include <QFile>
#include <QStringList>


namespace btm {

BtBookInterface::BtBookInterface(QObject* parent)
    : QObject(parent),
      m_key(0) {
}

static void outputText(const QString& text, const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << text;
    file.close();
}

QString BtBookInterface::getText() const {
    QString moduleName= getModuleName();
    QStringList moduleList = QStringList() << moduleName;

    QList<const CSwordModuleInfo*> modules =
        CSwordBackend::instance()->getConstPointerList(moduleList);
    Rendering::CEntryDisplay* display = modules.first()->getDisplay();

    if (m_key == 0)
        return "";

    QString key = m_key->key();
    if (key.isEmpty()) {
        m_key->firstChild();
        key = m_key->key();
    }

    QString anchor = Rendering::CDisplayRendering::keyToHTMLAnchor(key);

    QString gotoFunction = "function gotoAnchor(anchor) {document.location=document.location + \"#\" + anchor;}";
    QString beginScript = "<script  type=\"text/javascript\">";
    QString endScript = "</script>";
    QString script = beginScript + gotoFunction + endScript;
    QString endHeader = "</head>";
    QString body = "<body";
    QString bodyGoto = "<body onload=\"gotoAnchor('" +anchor + "')\"  ";

//    BtConfig & conf = btConfig();
//    conf.beginGroup(windowGroup);
    FilterOptions filterOptions;// = conf.getFilterOptions();
    DisplayOptions displayOptions;// = conf.getDisplayOptions();
    displayOptions.verseNumbers =1;
    displayOptions.lineBreaks = 1;
//    conf.endGroup();

    QString text;
    if (display) {
        text = display->text(modules, key, displayOptions, filterOptions);
    }

    text.replace(endHeader, script + endHeader);
    text.replace(body, bodyGoto);

    outputText(text, "out.html");
    return text;
}

QString BtBookInterface::getModuleName() const {
    QString moduleName;
    if (m_key)
        moduleName = m_key->module()->name();
    return moduleName;
}

void BtBookInterface::setModuleName(const QString& moduleName) {
    CSwordModuleInfo* m = CSwordBackend::instance()->findModuleByName(moduleName);
    if (!m_key) {
        CSwordKey* key = CSwordKey::createInstance(m);
        m_key = dynamic_cast<CSwordTreeKey*>(key);
    }
    else {
        m_key->setModule(m);
    }
    m_key->firstChild();
////    BtConfig & conf = btConfig();
////    conf.beginGroup(windowGroup);
//    FilterOptions filterOptions;// = conf.getFilterOptions();
//    DisplayOptions displayOptions;// = conf.getDisplayOptions();
////    conf.endGroup();

    emit moduleChanged();
    emit displayedChanged();
    emit textChanged();
}

QString BtBookInterface::getDisplayed() const {
    QString displayed;
    if (m_key)
        displayed = m_key->key();
    return displayed;
}

void BtBookInterface::changeModule() {
    QtQuick2ApplicationViewer* viewer = getViewManager()->getViewer();
    BookModuleChooser* dlg = new BookModuleChooser(viewer, this);
    dlg->open();
}

static void parseKey(CSwordTreeKey* currentKey, QStringList* keyPath, QStringList* children)
{
    if (currentKey == 0)
        return;

    CSwordTreeKey localKey(*currentKey);

    QString oldKey = localKey.key(); //string backup of key

    if (oldKey.isEmpty()) { //don't set keys equal to "/", always use a key which may have content
        localKey.firstChild();
        oldKey = localKey.key();
    }
    const int oldOffset = localKey.getOffset(); //backup key position

    QStringList siblings; //split up key
    if (!oldKey.isEmpty()) {
        siblings = oldKey.split('/', QString::SkipEmptyParts);
    }

    int depth = 0;
    int index = 0;
    localKey.root(); //start iteration at root node

    while ( localKey.firstChild() && (depth < siblings.count()) ) {
        QString key = localKey.key();
        index = (depth == 0) ? -1 : 0;

        bool found = false;
        do { //look for matching sibling
            ++index;
            found = (localKey.getLocalNameUnicode() == siblings[depth]);
        }
        while (!found && localKey.nextSibling());

        if (found)
            key = localKey.key(); //found: change key to this level
        else
            localKey.setKey(key); //not found: restore old key

        *keyPath << key;

        //last iteration: get child entries
        if (depth == siblings.count() - 1 && localKey.hasChildren()) {
            localKey.firstChild();
            ++depth;
            do {
                *children << localKey.getLocalNameUnicode();
            }
            while (localKey.nextSibling());
        }
        depth++;
    }
}

void BtBookInterface::chooseKey() {
    QtQuick2ApplicationViewer* viewer = getViewManager()->getViewer();
    QStringList keyPath;
    QStringList children;
    parseKey(m_key, &keyPath, &children);
    BookKeyChooser* dlg = new BookKeyChooser(viewer, this);
    dlg->open();
}

void BtBookInterface::setDisplayed(const QString& text) {
    emit displayedChanged();
    emit textChanged();
}

const CSwordBookModuleInfo* BtBookInterface::module() const {
    const CSwordModuleInfo* module = m_key->module();
    const CSwordBookModuleInfo* bibleModule = qobject_cast<const CSwordBookModuleInfo*>(module);
    return bibleModule;
}

CSwordTreeKey* BtBookInterface::getKey() const {
    return m_key;
}


} // end namespace