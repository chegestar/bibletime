/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime Mini project. Visit 
* http://sourceforge.net/projects/bibletimemini for more information.
*
* This code is licensed under the GNU General Public License version 2.0.
*
**********/

#include <QDesktopServices>
#include <QStyleFactory>
#include <QUrl>

#include <QtDebug>

#include <config.h>

#include "backend/config/cbtconfig.h"
#include "backend/cswordmodulesearch.h"

#include "btminisettingsmodel.h"
#include "view/btminilayoutdelegate.h"
#include "ui/btminimenu.h"

#define BT_MINI_FORUM_URL "http://sourceforge.net/p/bibletimemini/discussion"

enum BtMiniSettingsIds
{
    BtMiniFontSize = 1,
    BtMiniFontTextSize,
    BtMiniThreads,
    BtMiniContinuousScrolling,
#ifdef BT_MINI_WEBKIT
    BtMiniUseWebKit,
#endif
    BtMiniSearchType,
    BtMiniStyle,
    BtMiniForum = BtMiniStyle + 4
};


class BtMiniSettingsModelPrivate
{
public:

    BtMiniSettingsModelPrivate()
    {
        QString tbs = "<table width=\"100%\" cellpadding=\"5\"><tr><td>";

        _strings << "<body><font size=\"50%\"><center>Settings</center></font></body>";
        _strings << tbs + "Font size:</td> <td align=\"right\"><b>%1%</b></td></tr></table>";
        _strings << tbs + "Font size for text:</td> <td align=\"right\"><b>%1%</b></td></tr></table>";
        _strings << tbs + "Multi-threading:</td> <td align=\"right\"><b>%1</b></td></tr></table>";
        _strings << tbs + "Continuous scrolling:</td> <td align=\"right\"><b>%1</b></td></tr></table>";
#ifdef BT_MINI_WEBKIT
        _strings << tbs + "Use WebKit:</td> <td align=\"right\"><b>%1</b></td></tr></table>";
#endif
        _strings << tbs + "Search type:</td> <td align=\"right\"><b>%1</b></td></tr></table>";
        _strings << tbs + "Ui style:</td> <td align=\"right\"><b>%1</b></td></tr></table>";
        _strings << "<body><font size=\"50%\"><i>Note: most of settings will be applied after application restart.</i></font></body>";
        _strings << "<body><font size=\"50%\"><center>About</center></font></body>";
        _strings << "<body><b>BibleTime Mini</b> - spend your time with Bible on mobile!<br/>"
                    "<table width=\"100%\"><tr><td>Current version:</td><td align=\"right\"><b>"
                    BT_MINI_VERSION "</b></td></tr></table>"
                    "<table width=\"100%\"><tr><td>Built on:</td><td align=\"right\">" __DATE__ "</td></tr></table><br/><br/>"
                    "It is cross-platform open-source Bible study application designed for mobile devices.<br/><br/>"
                    "Underlying frameworks:"
                    "<table width=\"100%\"><tr><td>BibleTime:</td><td align=\"right\">2.9.1</td></tr></table>"
                    "<table width=\"100%\"><tr><td>Sword project:</td><td align=\"right\">" VERSION "</td></tr></table>"
                    "<table width=\"100%\"><tr><td>Qt framework:</td><td align=\"right\">" QT_VERSION_STR "</td></tr></table><br/>"
                    "</body>";
        _strings << "You could post feedback, report an issue or get help throught forum:"
                    "<br/><a href=\"" BT_MINI_FORUM_URL "\">" BT_MINI_FORUM_URL "</a></body>";

        Q_ASSERT(BtMiniForum == _strings.size() - 1);
    }

    ~BtMiniSettingsModelPrivate()
    {
        ;
    }

    QVector<QString>      _strings;
    BtMiniLayoutDelegate *_ld;
};

BtMiniSettingsModel::BtMiniSettingsModel(QObject *parent)
    : QAbstractItemModel(parent) , d_ptr(new BtMiniSettingsModelPrivate())
{
    d_ptr->_ld = new BtMiniLayoutDelegate(this);
    BtMiniLevelOption o = d_ptr->_ld->levelOption();
    o.scrollBarPolicy = Qt::ScrollBarAlwaysOn;
    d_ptr->_ld->setLevelOption(o);
}

BtMiniSettingsModel::~BtMiniSettingsModel()
{
    ;
}

int BtMiniSettingsModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int BtMiniSettingsModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const BtMiniSettingsModel);

    if(parent.isValid())
        return 0;

    return d->_strings.size();
}

QModelIndex BtMiniSettingsModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const BtMiniSettingsModel);

    Q_ASSERT(!parent.isValid());

    return createIndex(row, 0);
}

QModelIndex BtMiniSettingsModel::parent(const QModelIndex &index) const
{
    Q_D(const BtMiniSettingsModel);

    return QModelIndex();
}

QVariant BtMiniSettingsModel::data(const QModelIndex &index, int role) const
{
    Q_D(const BtMiniSettingsModel);

    Q_ASSERT(!index.parent().isValid());

    if (role == Qt::DisplayRole)
    {
        QString s(d->_strings[index.row()]);
        switch(index.row())
        {
        case BtMiniFontSize:
            s = s.arg(CBTConfig::get(CBTConfig::fontScale));
            break;
        case BtMiniFontTextSize:
            s = s.arg(CBTConfig::get(CBTConfig::fontTextScale));
            break;
#ifdef BT_MINI_WEBKIT
        case BtMiniUseWebKit:
            s = s.arg(CBTConfig::get(CBTConfig::useWebKit) ? "on" : "off");
            break;
#endif
        case BtMiniContinuousScrolling:
            s = s.arg(CBTConfig::get(CBTConfig::miniContinuousScrolling) ? "on" : "off");
            break;
        case BtMiniStyle:
            s = s.arg(CBTConfig::get(CBTConfig::miniStyle));
            break;
        case BtMiniThreads:
            s = s.arg(CBTConfig::get(CBTConfig::threadedTextRetrieving) ? "on" : "off");
            break;
        case BtMiniSearchType:
            switch(CBTConfig::get(CBTConfig::searchType))
            {
                case CSwordModuleSearch::AndType:
                    s = s.arg("AND");
                break;
                case CSwordModuleSearch::OrType:
                    s = s.arg("OR");
                break;
                case CSwordModuleSearch::FullType:
                    s = s.arg("Full syntax");
                break;
                default:
                    s = s.arg("Unknown");
            }
            break;
        default:
            ;
        }

        return s;
    }

    return QVariant();
}

bool BtMiniSettingsModel::hasChildren(const QModelIndex &parent) const
{
    return (rowCount(parent) > 0);
}

Qt::ItemFlags BtMiniSettingsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

QVariant BtMiniSettingsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

void BtMiniSettingsModel::clicked(const QModelIndex &index)
{
    Q_D(const BtMiniSettingsModel);

    switch(index.row())
    {
    case BtMiniFontSize:
        CBTConfig::set(CBTConfig::fontScale, BtMiniMenu::execInput(
            "Select size:", "<b>%1%</b>", CBTConfig::get(CBTConfig::fontScale), 1, 1000));
        emit dataChanged(index, index);
        break;

    case BtMiniFontTextSize:
        CBTConfig::set(CBTConfig::fontTextScale, BtMiniMenu::execInput(
            "Select size:", "<b>%1%</b>", CBTConfig::get(CBTConfig::fontTextScale), 1, 1000));
        emit dataChanged(index, index);
        break;

#ifdef BT_MINI_WEBKIT
    case BtMiniUseWebKit:
        CBTConfig::set(CBTConfig::useWebKit, !CBTConfig::get(CBTConfig::useWebKit));
        emit dataChanged(index, index);
        break;
#endif

    case BtMiniContinuousScrolling:
        CBTConfig::set(CBTConfig::miniContinuousScrolling, !CBTConfig::get(CBTConfig::miniContinuousScrolling));
        emit dataChanged(index, index);
        break;

    case BtMiniStyle:
        {
            QStringList ss(QStyleFactory::keys());
            int i = ss.indexOf(CBTConfig::get(CBTConfig::miniStyle));
            QString s;
            if(i >= 0 && i < ss.size() - 1)
                s = ss[i + 1];
            else
                s = ss[0];
            CBTConfig::set(CBTConfig::miniStyle, s);
            //emit dataChanged(index, index, QVector<int>());
            emit dataChanged(index, index);
        }
        break;

    case BtMiniForum:
        if(BtMiniMenu::execQuery("Follow link?", QStringList() << "Yes" << "No") == 0)
            QDesktopServices::openUrl(QUrl(BT_MINI_FORUM_URL));
        break;

    case BtMiniThreads:
        CBTConfig::set(CBTConfig::threadedTextRetrieving, !CBTConfig::get(CBTConfig::threadedTextRetrieving));
        emit dataChanged(index, index);
        break;

    case BtMiniSearchType:
        CBTConfig::set(CBTConfig::searchType, (CBTConfig::get(CBTConfig::searchType) + 1)
                       % (CSwordModuleSearch::FullType + 1));
        emit dataChanged(index, index);
    default:
        ; //qDebug() << "clicked" << index.data();
    }
}
