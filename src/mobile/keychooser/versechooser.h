/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef VERSE_CHOOSER_H
#define VERSE_CHOOSER_H

#include <QObject>
#include <QList>

class QtQuick2ApplicationViewer;
class QQmlComponent;
class QQuickItem;
class QStringList;

namespace btm {

class GridChooser;

class BtWindowInterface;

class VerseChooser : public QObject {
    Q_OBJECT

    enum State {
        CLOSED,
        BOOK,
        CHAPTER,
        VERSE
    };

public:
    VerseChooser(QtQuick2ApplicationViewer* viewer, BtWindowInterface* bibleVerse);
    void open();

private slots:
    void stringAccepted(const QString& value);
    void stringCanceled();

private:
    void showGridChooser(const QStringList& list);
    void setProperties(const QStringList& list);


    QtQuick2ApplicationViewer* m_viewer;
    GridChooser* m_gridChooser;
    BtWindowInterface* bibleVerse_;
    int m_state;
    QString m_oldBook;
    QString m_oldChapter;
    QString m_oldVerse;
    QString m_newBook;
    QString m_newChapter;
    QString m_newVerse;
};

} // end namespace

#endif