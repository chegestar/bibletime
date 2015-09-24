/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btconfigcore.h"

#include <cstddef>
#include <limits>

#define GROUP_SESSIONS      QString("sessions/")
#define KEY_CURRENT_SESSION QString("sessions/currentSession")
#define GROUP_SESSION       QString("sessions/%1/")
#define KEY_SESSION_NAME    QString("sessions/%1/name")
#define UI_FONT_SIZE        QString("ui/fontSize")



BtConfigCore::BtConfigCore(const QString & settingsFile)
    : m_mutex(QMutex::Recursive)
    , m_settings(settingsFile, QSettings::IniFormat)
{
    /**
      \todo Read UI language from settings, and initialize translator for tr()
            strings.
    */

    // Read all session keys and names:
    m_settings.beginGroup(GROUP_SESSIONS);
    const QStringList sessionKeys = m_settings.childGroups();
    m_settings.endGroup();
    Q_FOREACH (const QString & sessionKey, sessionKeys) {
        // Skip empty//keys just in case:
        if (sessionKey.isEmpty())
            continue;

        const QString fullKey = KEY_SESSION_NAME.arg(sessionKey);
        const QString sessionName = m_settings.value(fullKey).toString();
        if (!sessionName.isEmpty())
            m_sessionNames.insert(sessionKey, sessionName);
    }

    // Get current session key:
    m_currentSessionKey = m_settings.value(KEY_CURRENT_SESSION).toString();

    /*
      If no session with the current session key exists, default to the first
      session found. If no sessions were found, create a default session.
    */
    if (m_currentSessionKey.isEmpty()
        || !m_sessionNames.contains(m_currentSessionKey))
    {
        if (m_sessionNames.isEmpty()) {
            QString const & newSessionName =
                    QString::number(static_cast<qulonglong>(0u), 36);
            m_currentSessionKey = newSessionName;
            m_settings.setValue(KEY_CURRENT_SESSION, newSessionName);
            m_settings.setValue(KEY_SESSION_NAME.arg(newSessionName),
                                tr("Default Session"));
        } else {
            m_currentSessionKey = m_sessionNames.keys().first();
        }
    }
    m_cachedCurrentSessionGroup = GROUP_SESSION.arg(m_currentSessionKey);
}

void BtConfigCore::setCurrentSession(const QString & key) {
    Q_ASSERT(!key.isEmpty());

    QMutexLocker lock(&m_mutex);
    Q_ASSERT(m_sessionNames.contains(key));
    m_currentSessionKey = key;
    m_cachedCurrentSessionGroup = GROUP_SESSION.arg(key);

    m_settings.setValue(KEY_CURRENT_SESSION, key);
    m_settings.sync();
}

QString BtConfigCore::addSession(const QString & name) {
    Q_ASSERT(!name.isEmpty());

    // Generate a new session key:
    QString key = QString::number(0u, 36);
    QMutexLocker lock(&m_mutex);
    if (m_sessionNames.contains(key)) {
        QString keyPrefix;
        std::size_t i = 1u;
        for (;;) {
            key = QString::number(i, 36);
            if (!m_sessionNames.contains(keyPrefix + key))
                break;
            if (i == std::numeric_limits<std::size_t>::max()) {
                i = 0u;
                keyPrefix.append('_');
            } else {
                i++;
            }
        };
    }
    Q_ASSERT(!m_sessionNames.contains(key));
    m_sessionNames.insert(key, name);

    m_settings.setValue(KEY_SESSION_NAME.arg(key), name);
    m_settings.sync();
    return key;
}


void BtConfigCore::deleteSession(const QString & key) {
    QMutexLocker lock(&m_mutex);
    Q_ASSERT(m_sessionNames.contains(key));
    Q_ASSERT(key != m_currentSessionKey);
    m_sessionNames.remove(key);

    m_settings.remove(GROUP_SESSIONS + key);
    m_settings.sync();
}

QStringList BtConfigCore::childKeys() {
    QMutexLocker lock(&m_mutex);
    return childGroups__();
}

QStringList BtConfigCore::childKeys__() {
    if (m_groups.isEmpty())
        return m_settings.childKeys();

    m_settings.beginGroup(group());
    const QStringList gs = m_settings.childKeys();
    m_settings.endGroup();
    return gs;
}

QStringList BtConfigCore::childKeys(const QString & subkey) {
    beginGroup(subkey);
    QStringList gs = childKeys__();
    endGroup();
    return gs;
}

QStringList BtConfigCore::childGroups() {
    QMutexLocker lock(&m_mutex);
    return childGroups__();
}

QStringList BtConfigCore::childGroups__() {
    if (m_groups.isEmpty())
        return m_settings.childGroups();

    m_settings.beginGroup(group());
    const QStringList gs = m_settings.childGroups();
    m_settings.endGroup();
    return gs;
}

QStringList BtConfigCore::childGroups(const QString & subkey) {
    beginGroup(subkey);
    QStringList gs = childGroups__();
    endGroup();
    return gs;
}

QStringList BtConfigCore::sessionChildGroups() {
    QMutexLocker lock(&m_mutex);
    return sessionChildGroups__();
}

QStringList BtConfigCore::sessionChildGroups__() {
    QMutexLocker lock(&m_mutex);
    m_settings.beginGroup(m_cachedCurrentSessionGroup + group());
    const QStringList gs = m_settings.childGroups();
    m_settings.endGroup();
    return gs;
}

QStringList BtConfigCore::sessionChildGroups(const QString & subkey) {
    beginGroup(subkey);
    QStringList gs = sessionChildGroups__();
    endGroup();
    return gs;
}

void BtConfigCore::remove(const QString & key) {
    QMutexLocker lock(&m_mutex);
    if (m_groups.isEmpty()) {
        m_settings.remove(key);
    } else {
        m_settings.remove(m_groups.join("/") + '/' + key);
    }
}

void BtConfigCore::sessionRemove(const QString & key) {
    QMutexLocker lock(&m_mutex);
    if (m_groups.isEmpty()) {
        m_settings.remove(m_cachedCurrentSessionGroup + key);
    } else {
        m_settings.remove(m_cachedCurrentSessionGroup + m_groups.join("/") + '/' + key);
    }
}
