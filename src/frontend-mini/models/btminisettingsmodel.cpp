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

#include <QtDebug>

#include "backend/config/cbtconfig.h"

#include "btminisettingsmodel.h"
#include "view/btminilayoutdelegate.h"
#include "ui/btminimenu.h"

enum BtMiniSettingsIds
{
    BtMiniFontSize = 1,
    BtMiniFontTextSize,
    BtMiniContinuousScrolling,
    BtMiniUseWebKit,
    BtMiniStyle
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
        _strings << tbs + "Continuous scrolling:</td> <td align=\"right\"><b>%1</b></td></tr></table>";
#ifdef BT_MINI_WEBKIT
        _strings << tbs + "Use WebKit:</td> <td align=\"right\"><b>%1</b></td></tr></table>";
#endif
        _strings << tbs + "Ui style:</td> <td align=\"right\"><b>%1</b></td></tr></table>";
        _strings << "<body><font size=\"50%\"><i>Note: settings will be applied after application restart.</i></font></body>";
        //_strings << "<table width=\"100%\" bgcolor=\"#9cd2ff\"><tr><td><font size=\"50%\"><center>About</center></font></td></tr></table>";
        _strings << "<body><font size=\"50%\"><center>About</center></font></body>";
        _strings << "Please send feedback to <i>kalemas@mail.ru</i>";
        //_strings << "<body><font size=\"50%\"><center>About</center></font><br/>Please send feedback to <i>kalemas@mail.ru</i></body>";
    }

    ~BtMiniSettingsModelPrivate()
    {
        ;
    }

    QVector<QString>      _strings;
};

BtMiniSettingsModel::BtMiniSettingsModel(QObject *parent)
    : QAbstractItemModel(parent)
    , d_ptr(new BtMiniSettingsModelPrivate())
{
    ;
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
        case BtMiniStyle:
            s = s.arg("day");
            break;
        case BtMiniUseWebKit:
            s = s.arg(CBTConfig::get(CBTConfig::useWebKit) ? "on" : "off");
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

//bool BtMiniSettingsModel::setData(const QModelIndex &index, const QVariant &value, int role)
//{
//    return false;
//}

void BtMiniSettingsModel::clicked(const QModelIndex &index)
{
    Q_D(const BtMiniSettingsModel);

    switch(index.row())
    {
    case BtMiniFontSize:
        {
            CBTConfig::set(CBTConfig::fontScale, BtMiniMenu::execInput(
                "Select size:", "<b>%1%</b>", CBTConfig::get(CBTConfig::fontScale), 1, 1000));
            emit dataChanged(index, index);
        }
        break;
    case BtMiniFontTextSize:
        {
            CBTConfig::set(CBTConfig::fontTextScale, BtMiniMenu::execInput(
                "Select size:", "<b>%1%</b>", CBTConfig::get(CBTConfig::fontTextScale), 1, 1000));
            emit dataChanged(index, index);
        }
        break;
    case BtMiniContinuousScrolling:
        ;
        break;
    case BtMiniUseWebKit:
        {
            CBTConfig::set(CBTConfig::useWebKit, !CBTConfig::get(CBTConfig::useWebKit));
            emit dataChanged(index, index);
        }
        break;
    default:
        ;
    }
}
