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

#include "backend/config/btconfig.h"
#include "backend/cswordmodulesearch.h"

#include "btmini.h"
#include "view/btminiview.h"
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
#ifndef BT_NO_CLUCENE
    BtMiniSearchType,
#endif
    BtMiniStyle,
    BtMiniTips = BtMiniStyle + 1,
    BtMiniNews,
    BtMiniForum = BtMiniNews + 3
};


class BtMiniSettingsModelPrivate
{
public:

    BtMiniSettingsModelPrivate()
    {
        QString tbs = "<table width=\"100%\" cellpadding=\"5\"><tr><td>";

        _strings << "<body><font size=\"50%\"><center>" + BtMiniSettingsModel::tr("Settings") +
                    "</center></font></body>";
        _strings << tbs + BtMiniSettingsModel::tr("Font size:") +
                    "</td> <td align=\"right\"><b>%1%</b></td></tr></table>";
        _strings << tbs + BtMiniSettingsModel::tr("Font size for text:") +
                    "</td> <td align=\"right\"><b>%1%</b></td></tr></table>";
        _strings << tbs + BtMiniSettingsModel::tr("Multi-threading:") +
                    "</td> <td align=\"right\"><b>%1</b></td></tr></table>";
        _strings << tbs + BtMiniSettingsModel::tr("Continuous scrolling:") +
                    "</td> <td align=\"right\"><b>%1</b></td></tr></table>";
#ifdef BT_MINI_WEBKIT
        _strings << tbs + BtMiniSettingsModel::tr("Use WebKit:") +
                    "</td> <td align=\"right\"><b>%1</b></td></tr></table>";
#endif
#ifndef BT_NO_CLUCENE
        _strings << tbs + BtMiniSettingsModel::tr("Search type:") +
                    "</td> <td align=\"right\"><b>%1</b></td></tr></table>";
#endif
        _strings << tbs + BtMiniSettingsModel::tr("Ui style:") +
                    "</td> <td align=\"right\"><b>%1</b></td></tr></table>";
        _strings << "<b>" + tbs + BtMiniSettingsModel::tr("Handbook:") +
                    "</td> <td align=\"right\"> > </td></tr></table></b>";
        _strings << "<b>" + tbs + BtMiniSettingsModel::tr("News and Updates:") +
                    "</td> <td align=\"right\"> > </td></tr></table></b>";
        _strings << "<body><font size=\"50%\"><center>" +
                    BtMiniSettingsModel::tr("About") + "</center></font></body>";
        _strings << "<body>" + BtMiniSettingsModel::tr("<b>BibleTime Mini</b> - spend your time with Bible on mobile!") +
                    "<br/><table width=\"100%\"><tr><td>" + BtMiniSettingsModel::tr("Current version:") +
                    "</td><td align=\"right\"><b>" BT_MINI_VERSION "</b></td></tr></table>"
                    "<table width=\"100%\"><tr><td>" + BtMiniSettingsModel::tr("Built on:") + "</td><td align=\"right\">"
                    __DATE__ "</td></tr></table><br/><br/>" +
                    BtMiniSettingsModel::tr("It is cross-platform open-source Bible study application designed for mobile devices.") +
                    "<br/><br/>" +
                    BtMiniSettingsModel::tr("Underlying frameworks:") +
                    "<table width=\"100%\"><tr><td>BibleTime:</td><td align=\"right\">2.10.dev</td></tr></table>"
                    "<table width=\"100%\"><tr><td>Sword project:</td><td align=\"right\">" VERSION "</td></tr></table>"
                    "<table width=\"100%\"><tr><td>Qt framework:</td><td align=\"right\">" QT_VERSION_STR "</td></tr></table>"
                    "</body>";
        _strings << BtMiniSettingsModel::tr("You could post feedback, report an issue or get help throught forum:") +
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

    if(parent.internalId() == 0)
    {
        if(parent.row() == BtMiniTips ||
           parent.row() == BtMiniNews)
            return 1;
    }

    if(parent.isValid())
        return 0;

    return d->_strings.size();
}

QModelIndex BtMiniSettingsModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const BtMiniSettingsModel);

    if(parent.isValid())
        return createIndex(row, 0, parent.row());

    return createIndex(row, 0);
}

QModelIndex BtMiniSettingsModel::parent(const QModelIndex &index) const
{
    Q_D(const BtMiniSettingsModel);

    if(index.internalId() != 0)
        return createIndex(index.internalId(), 0);

    return QModelIndex();
}

QVariant BtMiniSettingsModel::data(const QModelIndex &index, int role) const
{
    Q_D(const BtMiniSettingsModel);

    if (role == Qt::DisplayRole)
    {
        if(index.internalId() == BtMiniTips)
            return standardData(tipWorks);
        if(index.internalId() == BtMiniNews)
            return standardData(news);

        QString s(d->_strings[index.row()]);
        switch(index.row())
        {
        case BtMiniFontSize:
            s = s.arg(btConfig().value<int>("mini/fontScale", 100));
            break;
        case BtMiniFontTextSize:
            s = s.arg(btConfig().value<int>("mini/fontTextScale", 100));
            break;
#ifdef BT_MINI_WEBKIT
        case BtMiniUseWebKit:
            s = s.arg(btConfig().value<bool>("mini/useWebKit", false) ? tr("on") : tr("off"));
            break;
#endif
        case BtMiniContinuousScrolling:
            s = s.arg(btConfig().value<bool>("mini/miniContinuousScrolling", false) ? tr("on") : tr("off"));
            break;
        case BtMiniStyle:
            s = s.arg(btConfig().value<QString>("mini/miniStyle", "mini"));
            break;
        case BtMiniThreads:
            s = s.arg(btConfig().value<bool>("mini/threadedTextRetrieving", true) ? tr("on") : tr("off"));
            break;
#ifndef BT_NO_CLUCENE
        case BtMiniSearchType:
            switch(btConfig().value<int>("GUI/SearchDialog/searchType", CSwordModuleSearch::AndType))
            {
                case CSwordModuleSearch::AndType:
                    s = s.arg(tr("AND"));
                break;
                case CSwordModuleSearch::OrType:
                    s = s.arg(tr("OR"));
                break;
                case CSwordModuleSearch::FullType:
                    s = s.arg(tr("Full syntax"));
                break;
                default:
                    s = s.arg("Unknown");
            }
            break;
#endif
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

QVariant BtMiniSettingsModel::standardData(StandardData data)
{
    switch (data) {
    case tipWorks:
        return QString("<body>" + tr("<b><center>Usefull tips:</center></b><br/>") +
            tr("Many views in BibleTime Mini recognizes <b>short pressing</b> by finger and <b>long pressing</b>. "
               "Try to press screen on word you interested in for a second (device should vibrate ones), "
               "information (context) view for that word should open. Same for footnotes and cross-references.<br/>"
               "Try to press Text view for several seconds before device vibrates twice, service menu should open. "
               "Where you will be able to add and remove additional views for parallel (currently unsyncronized) viewing.") + "<br/><br/>" +
            tr("<b>Context view.</b> It is consist of two views: word context, and commentary context. Try to slide left/right "
               "to observe them. Default modules used for this context could be configurated in Module selection window "
               "by long pressping.") + "<br/><br/>" +
            tr("<b>Search.</b> BibleTime Mini uses CLucene indexed search, this means that you have to wait for some "
               "time to generate search index database to search in the module (on 1GHz device to index heavy module like KJV, "
               "it takes for 10 minutes and on 600MHz devices same take for about hour). Switch to Find view and type to "
               "input box at the top your request. Try to use wildcards \"*\" (like \"take*\"), keywords (like \"strong:G1203\" "
               "or \"footnote:Eve\"), and combine several words. Note, you should have keyboard with Enter key on your keyboard to "
               "start search, or on some platforms there is way to insert Return Carriage symbol.<br/>Long pressing on item will bring "
			   "verse to reading window.") + "<br/><br/>" +
            tr("<b>Default Modules.</b> There are several types of modules: default Bible is used to render cross-references, "
               "default Commentary, to show in context popup, Greek and Hebrew Strongs Dictionaries to render definitions in "
               "context popup. To change default modules, when you open Module Selection popup, make long press on module you "
               "would like to make default and select corresponding action.") + "<br/><br/>" +
            tr("<b>Parallel Bible display.</b> You would open several Bible texts in one view, with respect of different verse "
               "markups. On any view with Bible opened, open module selection dialog and make long press on the module you would like "
               "to add, then select \"") + tr("Add Parallel") + "\"." +
            QString("</body>"));
    case tipWorksAddon:
        return QString(tr("<br/>This information you could access again in <b>Settings</b> under the <b>Handbook</b> item."));
    case news:
        return QString(tr("<body><b><center>News and Updates:</center></b><br/>") +
                       tr("<b>0.9.5</b> - parallel Bible display, make long tap in module selection dialog "
                          "and press \"") + tr("Add Parallel") + "\". This feature is not well tested and may not work "
                          "as expected.<br/>" +
                       tr("<b>0.9.3</b> - was added support for General Books and search in non Bible "
                          "modules was fixed.") +
                       "</body>");
    default:
        return QVariant();
    }
}

void BtMiniSettingsModel::clicked(const QModelIndex &index)
{
    Q_D(const BtMiniSettingsModel);

    switch(index.row())
    {
    case BtMiniFontSize:
        {
            int ov = btConfig().value<int>("mini/fontScale", 100);
            int nv = BtMiniMenu::execInput(tr("Select size:"), "<b>%1%</b>", ov, 1, 1000);

            if(ov != nv)
            {
                btConfig().setValue("mini/fontScale", nv);
                emit dataChanged(index, index);

				BtMini::mainWidget(true);
				BtMini::worksWidget(true, true);
				BtMini::installerWidget(false, true);
				BtMini::settingsWidget(true);
				BtMini::searchWidget(true);
            }
        }
        break;

    case BtMiniFontTextSize:
		{
			int ov = btConfig().value<int>("mini/fontTextScale", 100);
			int nv = BtMiniMenu::execInput(tr("Select size:"), "<b>%1%</b>", ov, 1, 1000);

			if(ov != nv)
			{
				btConfig().setValue("mini/fontTextScale", nv);
				emit dataChanged(index, index);

				BtMini::worksWidget(true, true);
				BtMini::installerWidget(false, true);
				BtMini::settingsWidget(true);
				BtMini::searchWidget(true);
			}
		}
        break;

#ifdef BT_MINI_WEBKIT
    case BtMiniUseWebKit:
        btConfig().setValue("mini/useWebKit", !btConfig().value<bool>("mini/useWebKit", false));
        emit dataChanged(index, index);

        BtMini::worksWidget(true, true);
        BtMini::searchWidget(true);
        break;
#endif

    case BtMiniContinuousScrolling:
        {
            bool b = !btConfig().value<bool>("mini/miniContinuousScrolling", false);
            btConfig().setValue("mini/miniContinuousScrolling", b);
            BtMini::findView(BtMini::worksWidget())->setContinuousScrolling(b);
            emit dataChanged(index, index);
        }
        break;

    case BtMiniStyle:
        {
            QStringList ss(QStyleFactory::keys());
            int i = ss.indexOf(btConfig().value<QString>("mini/miniStyle", "mini"));
            QString s;
            if(i >= 0 && i < ss.size() - 1)
                s = ss[i + 1];
            else
                s = ss[0];
            btConfig().setValue("mini/miniStyle", s);
            emit dataChanged(index, index);

            BtMini::mainWidget(false, s);
            BtMini::worksWidget(true, true);
            BtMini::installerWidget(false, true);
            BtMini::settingsWidget(true);
            BtMini::searchWidget(true);
        }
        break;

    case BtMiniForum:
        if(BtMiniMenu::execQuery(tr("Follow link?"), QStringList() << tr("Yes") << tr("No")) == 0)
            QDesktopServices::openUrl(QUrl(BT_MINI_FORUM_URL));
        break;

    case BtMiniThreads:
        btConfig().setValue("mini/threadedTextRetrieving", !btConfig().value<int>("mini/threadedTextRetrieving", true));
        emit dataChanged(index, index);

        BtMini::worksWidget(true, true);
        break;

#ifndef BT_NO_CLUCENE
    case BtMiniSearchType:
        btConfig().setValue("GUI/SearchDialog/searchType", (btConfig().value<int>("GUI/SearchDialog/searchType",
                            CSwordModuleSearch::AndType) + 1) % (CSwordModuleSearch::FullType + 1));
        emit dataChanged(index, index);
		break;
#endif

    default:
        ;
    }
}
