/*************************************************************************
 * state.cpp - current screen state controller
 *
 * author: Konstantin Maslyuk "Kalemas" mailto:kalemas@mail.ru
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 ************************************************************************/

#include <algorithm>

#include <QApplication>
#include <QCheckBox>
#include <QDateTime>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QStyle>
#include <QtCore/qmath.h>

#include "backend/config/cbtconfig.h"
#include "backend/managers/cswordbackend.h"
#include "backend/rendering/centrydisplay.h"
#include "backend/cswordmodulesearch.h"
#include "btglobal.h"

#include <swversion.h>

#include "core.h"
#include "item.h"
#include "state.h"
#include "ui/button.h"
#include "ui/menu.h"
#include "utility/rendercache.h"
#include "utility/installstatus.h"

#pragma warning(disable:4018)

RenderCache DrawCache;

class sbListBound
{
public:
    int                   _lastItemId;
    int                   _countVerse;
    bool                  _constructed;
    int                   _pixelHeight;
};

class sbListPrivate
{
public:
    sbListPrivate()
    {
        _filterOptions = CBTConfig::getFilterOptionDefaults();
        _displayOptions = CBTConfig::getDisplayOptionDefaults();
    }

    ~sbListPrivate()
    {
        ;
    }

    void addItem(const int pos, sbItem *item)
    {
        ;
    }

    void removeItem(const int pos)
    {
        ;
    }


    void setItemHeight(const int pos, const int height)
    {
        ;
    }

    static void searchProgress(char percent, void *data)
    {
        QProgressDialog *dialog = reinterpret_cast<QProgressDialog*>(data);
        if(dialog)
        {
            QApplication::processEvents();
            if(dialog->wasCanceled())
                Core_->currentModule->terminateSearch = true;
            dialog->setValue(percent);
        }
    }

    QList<sbItem*> _items;
    sbListBound    _boundTop;
    sbListBound    _boundBottom;
    QStringList    _controls;
	QStringList    _indicators;

	DisplayOptions _displayOptions;
	FilterOptions  _filterOptions;

	int            _separatorHeight;
};

sbList::sbList(View::Type _type_) : d_ptr(new sbListPrivate), type(_type_)
{
    Q_D(sbList);

	itemCurrent = itemHover = itemSelected = itemBanner = NULL;
	next = prev = NULL;
	displace = 0.0f;
	_module = NULL;
	attachedReading = attachedHistory = -1;

    rect = Core_->getContentsWidget()->rect();

	d->_separatorHeight = qMin(rect.width(), rect.height()) * 0.008f;

	switch(type)
	{
	case View::TYPE_SELECT_MODULE:
		d->_controls << "open search" << "open book selection" << "back";
		break;
	case View::TYPE_SELECT_BOOK:
        d->_controls << "open search" << "open module selection" << "back";
		break;
	case View::TYPE_SEARCH:
        d->_controls << "search: input" << "open book selection" << "open module selection" << "back";
		d->_indicators << "search: label";
		break;
	case View::TYPE_MODULE_INSTALLER_DISCLAIMER:
        d->_controls << "moduleinstaller: accept" << "moduleinstaller: decline";
		break;
	case View::TYPE_MODULE_INSTALLER:
        d->_controls << "back";
		break;
	case View::TYPE_MODULE_VIEW:
        d->_controls << "open navigation" << "open options" << "home";
        d->_indicators << "arrow left" << "module: label" << "arrow right";
		after.constructed = before.constructed = false;
		break;
	case View::TYPE_READINGS:
	case View::TYPE_HISTORY:
        d->_controls << "back";
		break;
	case View::TYPE_MENU_OPTIONS:
        d->_controls << "back";
		break;
	case View::TYPE_HOME:
		d->_controls << "open navigation" << "open history" << "exit";
		break;
	default:
		Q_ASSERT(false);
	}
}

sbList::~sbList()
{
	clear();

	if (next != NULL)
		next->prev = prev;
	if (prev != NULL)
		prev->next = next;

	if (itemBanner)
        delete itemBanner;
}

void sbList::attach(sbList *list, View::Type to)
{
	switch(to)
	{
    case View::PIN_PREV:
		if (prev != NULL)
		{
			list->prev = prev;
			prev->next = list;
		}
        list->next = this;
		prev = list;
		break;

	case View::PIN_NEXT:
		if (next != NULL)
		{
			list->next = next;
			next->prev = list;
		}
		list->prev = this;
		next = list;
		break;

	default:
		Q_ASSERT(false);
	}
}

/*   sbList::clear
 *   empty list from items
 */
void sbList::clear()
{
	if (itemCurrent != NULL)
	{
		while (itemCurrent->prev != NULL)
		{
			DrawCache.remove(itemCurrent->prev);
			delete itemCurrent->prev;
		}
		while (itemCurrent->next != NULL)
		{
			DrawCache.remove(itemCurrent->next);
			delete itemCurrent->next;
		}
		DrawCache.remove(itemCurrent);
		delete itemCurrent;

		itemCurrent    = NULL;
		before = after = BOUND();
		displace       = 0;

        if(type == View::TYPE_MODULE_VIEW)
            before.constructed = after.constructed = false;
	}
}

/*  sbList::render
 *  draw list
 */
void sbList::render(QPainter *painter, int sideDisplacement)
{
	Q_D(sbList);

    // itemCurrent may be not first on screen, so do correction
	if(itemCurrent != NULL)
	{
		while(displace > -d->_separatorHeight && itemCurrent->prev != NULL)
		{
			if(itemCurrent->type == sbItem::TYPE_VERSE)
			{
				before.verseCount--;
				after.verseCount++;
			}

			itemCurrent = itemCurrent->prev;
			displace -= itemCurrent->size().height() + d->_separatorHeight;
		}

		while(displace < 0 && qAbs(displace) > itemCurrent->size().height() + d->_separatorHeight)
		{
			if(itemCurrent->next == NULL)
                break;

			displace += itemCurrent->size().height() + d->_separatorHeight;

			if (itemCurrent->type == sbItem::TYPE_VERSE)
			    before.verseCount++, after.verseCount--;

			itemCurrent = itemCurrent->next;
		}
	}

	sbItem *oldItem;
	sbItem *workItem = itemCurrent;
	QRect workRect;
	QRect selectedRect;
    QRect rect = Core_->getContentsWidget()->rect();
	int screenHeight = static_cast<int>(displace);

	if(sideDisplacement <= 0)
		DrawCache.prepare();

	while(true)
	{
		if(workItem == NULL)
            break;
		if(screenHeight > rect.height())
			break;

		oldItem = workItem;
		workRect = rect.translated(sideDisplacement, 0);

		// horizontal parse
		while(workItem != NULL)
		{
			QPoint xy(workRect.left(), screenHeight);
			workRect = QRect(xy, workItem->size()) & rect;

			if(!workRect.isNull())
			{
                if(workItem->highlighted)
                {
                    painter->setBrush(QApplication::palette().highlight());
                    painter->drawRect(workRect);
                }
				if (workItem->type != sbItem::TYPE_VERSE || workItem->text.size() == 0 || workItem->expanding != 0 || !workItem->processed)
					workItem->render(painter, xy, workRect);
				else if (!DrawCache.blit(painter, workItem, xy, workRect))
					workItem->render(painter, xy, workRect);
				if(workItem == itemSelected)
					selectedRect = workRect;
			}
			workRect.translate(workItem->size().width(), 0);
			workItem = workItem->right;
		}
		workItem = oldItem;

		if(d->_separatorHeight > 0 && workItem->next != NULL)
		{
			workRect = rect.translated(sideDisplacement, workRect.bottom()) & rect;

            if(!workRect.isNull())
            {
			    workRect.setHeight(d->_separatorHeight);
				painter->setBrush(QApplication::palette().window());
				painter->drawRect(workRect);
			    screenHeight += d->_separatorHeight;
            }
		}

		screenHeight += workItem->size().height();

		workItem = workItem->next;
	}

	// scrolling
	if(itemSelected == NULL)
	{
		if(displace >= 0)
		{
			needScroll = static_cast<int>(-displace);
			needFill = displace != 0 || screenHeight < rect.height();
		}
		else if(screenHeight < rect.height())
		{
			needScroll = rect.height() - screenHeight;
			needFill = true;
		}
		else
		{
			needScroll = 0;
			needFill = false;
		}

		if((needScroll > 0 && !after.constructed) || (needScroll < 0 && !before.constructed))
            needScroll = 0;
	}
	else
	{
		if(selectedRect.width() > 0)
			needScroll = ((rect.height()/2)-((selectedRect.height()/2)+selectedRect.top()));
	}
}

/*   sbList::scroll
 *   vertical list scrolling
 *   return value indicates done work and necessity of redraw
 */
bool sbList::scroll(float amount)
{
	Q_D(sbList);

	//qDebug() << "sbList scroll " << amount;

	if(itemCurrent == NULL)
        return false;

	int oldDisplace = static_cast<int>(displace);

	// don't allow scroll further then list is
	if(amount < 0 && itemCurrent->next == NULL && itemCurrent->size().height()+d->_separatorHeight+displace+amount < 0)
	{
		amount = -(itemCurrent->size().height()+d->_separatorHeight+displace);
		Q_ASSERT(amount <= 0);
	}
	if(amount > 0 && itemCurrent->prev == NULL)
	{
		amount = qMin(amount, rect.height()-displace);
		Q_ASSERT(amount >= 0);
	}

	if(qAbs(amount) > rect.height()/3)
		amount = amount > 0.0f ? rect.height()/3.0f : -rect.height()/3.0f;
	
	displace += amount;

	if((int)displace == (int)oldDisplace )
        return false;
	return true;
}

/*   sbList::getItem
 *   get item by coordinates on screen
 */
sbItem* sbList::getItem(int x, int y) const
{
	if(!itemCurrent)
        return 0;

	sbItem *i = itemCurrent;
	int top = (int)displace;
	int left = 0;

	while(i != NULL && top < rect.height())
	{
		if(y > top && y < top + i->size().height())
		{
			int l = left;
			
			while(i != NULL)
			{
				if(x > l && x < l + i->size().width())
					return i;

				l += i->size().width();
				i = i->right;
			}

			return NULL;
		}
		top += i->size().height();
		top += d_ptr->_separatorHeight;

		i = i->next;
	}
	return NULL;
}

/*   sbList::onPressed
 *   input
 */
void sbList::onPressed( int x, int y)
{
	itemHover = getItem(x, y);
	itemSelected = 0;

	if(itemHover)
	{
		if(itemHover->type == sbItem::TYPE_VERSE)
		{
			// update current verse
			_place.Index(itemHover->index);
			Core_->currentVerse.copyFrom(_place);

            QLabel *l = Core_->getHeaderWidget()->findChild<QLabel*>("module: label");
            if(l)
                l->setText(QString("%1: %2").arg(_module).arg(_place.getShortText()));

			if(attachedHistory >= 0)
                Core_->features.updateHistory(attachedHistory, _place.Index());
		}
        Core_->getContentsWidget()->update();
	}
}

/*   sbList::onReleased
 *   input
 */
void sbList::onReleased(int x, int y)
{
	if (itemHover)
	{
		itemHover = 0;
		Core_->getContentsWidget()->update();
	}
}

/*   sbList::clicked
 *   item clicked, process action by item type
 */
void sbList::onClicked(int x, int y)
{
    sbItem *item = getItem(x, y);
	if (item)
		onItem(item);
}

void sbList::onItem(sbItem *item)
{
	switch (item->type)
	{
	case sbItem::TYPE_MODULE_INSTALLER_MODULE:
	case sbItem::TYPE_MODULE_INSTALLER_UPDATE:
		{
			sword::InstallSource *source = NULL;

			for(sbItem *i = item; i != NULL; i = i->prev)
				if(i->type == sbItem::TYPE_MODULE_INSTALLER_REPO)
				{
					source = (sword::InstallSource *)i->index;
					break;
				}

			Q_ASSERT(source);

			int success = -1;

			sword::SWModule *module = reinterpret_cast<sword::SWModule*>(item->index);

			if(Menu::execQuery(QString("Dou you want to install %1 of size %2 KB ?").arg(module->Name()).arg(module->getConfigEntry("InstallSize")), \
			    QStringList() << "Yes" << "No") == 0)
			{
				QProgressDialog *dialog = new QProgressDialog(Core_->getMainWidget());

				dialog->setAutoClose(false);
				dialog->setCancelButtonText("Cancel");
				dialog->setLabelText(QString("Installing %1").arg(module->Name()));
				dialog->show();
				dialog->repaint();

				Core_->installStatus->setDialog(dialog);

				success = Core_->getInstallMgr()->installModule(Core_->getSword(), "", module->Name(), source);

				delete dialog;

				if (success >= 0)
				{
                    // TODO reinit sword
					Core_->switchList(View::TYPE_SELECT_MODULE);
				}
				else
				{
				    Menu::execQuery(QString("Failed to install %1 (error code: %2)").arg(module->Name()).arg(success));
				}
			}

			//delete menu;
		}
		break;

	case sbItem::TYPE_MODULE_INSTALLER_REPO:
		{
			sword::InstallSource *source = (sword::InstallSource *)item->index;

			// repository expand / collapse
			if(item->next == NULL || item->next->type == sbItem::TYPE_MODULE_INSTALLER_REPO)
			{
				sword::StringList sl;
				std::map<sword::SWModule*, int> diff = Core_->getInstallMgr()->getModuleStatus(*Core_->getSword(), *source->getMgr());
				sbItem *lastItem = item;

				for(std::map<sword::SWModule*, int>::iterator it = diff.begin(); it != diff.end(); it++)
				{
					if (std::find(sl.begin(), sl.end(), (*it).first->Type()) == sl.end())
					{
						sl.push_back((*it).first->Type());
						lastItem->attach(sbItem::create(sbItem::LOOK_SEPARATOR, QString((*it).first->Type()),rect.width()),DIRECTION_NEXT);
						lastItem = lastItem->next;

						for(std::map<sword::SWModule*, int>::iterator im = it; im != diff.end(); im++)
						{
							if (!strcmp((*it).first->Type(),(*im).first->Type()))
							{
								if ( (*im).second & sword::InstallMgr::MODSTAT_NEW )
								{
									lastItem->attach(new sbItem(sbItem::TYPE_MODULE_INSTALLER_MODULE, QString("%1\n<s t=\"6\">%2</s>").arg((*im).first->Name()).arg((*im).first->Description()), rect.width(), (long)(*im).first), DIRECTION_NEXT);
									lastItem = lastItem->next;
								}
								else if((*im).second & sword::InstallMgr::MODSTAT_UPDATED)
								{
									lastItem->attach(new sbItem(sbItem::TYPE_MODULE_INSTALLER_UPDATE, QString("%1<s t=\"6\"> - Update\n%2</s>").arg((*im).first->Name()).arg((*im).first->Description()),rect.width(), (long)(*im).first), DIRECTION_NEXT);
									lastItem = lastItem->next;
								}
							}
						}
					}
				}
			}
			else
			{
				while (item->next != NULL && (item->next->type == sbItem::LOOK_SEPARATOR || item->next->type == sbItem::TYPE_MODULE_INSTALLER_MODULE || item->next->type == sbItem::TYPE_MODULE_INSTALLER_UPDATE) )
					delete item->next;
			}
		}
		break;

	case sbItem::TYPE_OPEN_MODULE_INSTALLER:
		{
			if (!Core_->getInstallMgr()->isUserDisclaimerConfirmed())
				Core_->switchList(View::TYPE_MODULE_INSTALLER_DISCLAIMER);
			else
				Core_->switchList(View::TYPE_MODULE_INSTALLER);
		}
		break;

	case sbItem::TYPE_SWITCH_MODULE_OPTION:
		{
			// set next option value
			sword::StringList sl = Core_->getSword()->getGlobalOptionValues(Core_->moduleOptions[item->index]);
			const char * co = Core_->getSword()->getGlobalOption(Core_->moduleOptions[item->index]);
			sword::StringList::iterator it = std::find(sl.begin(),sl.end(),co);

			if (it == sl.end() || ++it == sl.end()) it = sl.begin();

			Core_->getSword()->setGlobalOption(Core_->moduleOptions[item->index], (*it));

			// update text
			item->setText(QString("%1 : %2").arg(Core_->moduleOptions[item->index]).arg(Core_->getSword()->getGlobalOption(Core_->moduleOptions[item->index])));
		}
		break;

	case sbItem::TYPE_GOTO_MODULES:
		{
			Core_->switchList(View::TYPE_MODULE_VIEW);
		}
		break;

	case sbItem::TYPE_HISTORY:
	case sbItem::TYPE_GO_HISTORY:
		{
			Core_->goVerse       = true;
			Core_->currentModule = Core_->getSword()->getModule(Core_->features.history[item->index].module);
			Core_->currentVerse.copyFrom (Core_->currentModule->getKey());
			Core_->currentVerse.Index ( Core_->features.history[item->index].placeEnd );

            Core_->switchList(View::TYPE_MODULE_VIEW);
		}
		break;

	case sbItem::TYPE_BUTTON_BOOKMARK:
	case sbItem::TYPE_BOOKMARK_VERSE_NUMBER:
		{
			Core_->goVerse = true;
			if(Core_->currentModule == NULL)
			{
				Core_->currentModule = Core_->defaultModule;
				Core_->currentVerse.copyFrom(Core_->defaultModule->getKey());
			}
			Core_->currentVerse.Index(item->index);

			Core_->switchList(View::TYPE_MODULE_VIEW);
		}
		break;

	case sbItem::TYPE_BUTTON_SELECTION_BOOK:
		{
			if (item->next != NULL && item->next->type == sbItem::TYPE_BUTTON_SELECTION_CHAPTER)
			{
				// remove chapters
				while (item->next != NULL && item->next->type == sbItem::TYPE_BUTTON_SELECTION_CHAPTER)
					delete item->next;
			}
			else
			{
				// add chapters
				sbItem *tmpItem = item;
				sword::VerseKey workKey(Core_->currentVerse);
				workKey.Index(item->index);

				for(int i=0; i < workKey.getChapterMax(); ++i)
				{
					workKey.setChapter(i+1);

					sbItem *item = new sbItem(sbItem::TYPE_BUTTON_SELECTION_CHAPTER, QString("<div class=\"item\"><table width=100%><tr><td><h3><center>%1</center></h3></td></tr></table></div>").arg(workKey.getChapter()), rect.width()/3, workKey.Index());

					if(tmpItem->type == sbItem::TYPE_BUTTON_SELECTION_CHAPTER &&
						(tmpItem->right == NULL || tmpItem->right->right == NULL))
					{
						sbItem *last = tmpItem;
						while(last->right != NULL)
                            last = last->right;
						last->attach(item, DIRECTION_RIGHT);
					}
					else
					{
						tmpItem->attach (item, DIRECTION_NEXT);
						tmpItem = tmpItem->next;
					}
				}

				// add space
				if(tmpItem->type == sbItem::TYPE_BUTTON_SELECTION_CHAPTER)
				{
					if(tmpItem->right == NULL)
						tmpItem->attach(new sbItem (sbItem::DO_NONE, QString(), rect.width()/3*2), DIRECTION_RIGHT);
					else if(tmpItem->right->right == NULL)
						tmpItem->right->attach(new sbItem (sbItem::DO_NONE, QString(), rect.width()/3), DIRECTION_RIGHT);
				}
			}
		}
		break;

	case sbItem::TYPE_BUTTON_SELECTION_CHAPTER:
		{
            sword::VerseKey verse(Core_->currentVerse);
			verse.Index(item->index);

			QScopedPointer<Menu> dialog(new Menu);

            QFont f = dialog->font();
            f.setPixelSize(f.pixelSize() * 0.8);
            dialog->setFont(f);

            const QSize ms(QFontMetrics(f).width("100"), f.pixelSize()*1.2);

            QVBoxLayout *vertical = new QVBoxLayout;
            QGridLayout *layout = new QGridLayout;

            QLabel *l = new QLabel("Select Verse:", dialog.data());
            f = l->font();
            f.setBold(false);
            f.setPixelSize(f.pixelSize() * 0.5);
            l->setFont(f);
            l->setAlignment(Qt::AlignCenter);

            const int count = verse.getVerseMax();

            const int m = dialog->style()->pixelMetric(QStyle::PM_MenuPanelWidth);
            const QRect r = rect.adjusted(m, m + l->height(), -m, -m);

            const int mw = r.width()/ms.width() + 1;
            const int mh = r.height()/ms.height() + 1;
            
            const float step = qMax(count/(float)(mw*mh), 1.0f);
            const int ww = qCeil(qSqrt(count/step)) + (qCeil(count/step)-(qCeil(qSqrt(count/step))*mh))/mh;

            for(float i = 0; i < count; i += step)
            {
                verse.setVerse(qFloor(i+1));
                Button *b = new Button(QString::number(verse.getVerse()), QVariant(QString::fromUtf8(verse.getText())), dialog.data());

				b->setMaximumSize(ms);
				b->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

                QObject::connect(b, SIGNAL(clicked(QVariant)), Core_, SLOT(setModulePlace(QVariant)));
                QObject::connect(b, SIGNAL(clicked()), dialog.data(), SLOT(hide()));

                layout->addWidget(b, qRound(i/step)/ww, qRound(i/step)%ww);
            }

            vertical->addWidget(l, Qt::AlignCenter);
            vertical->addLayout(layout);

			dialog->setLayout(vertical);
            dialog->exec();
		}
		break;

	case sbItem::TYPE_BUTTON_SELECTION_MODULE:
		{
			sword::SWModule *module = reinterpret_cast<sword::SWModule*>(item->index);

			Q_ASSERT(module);

			// highlighting
			sbItem *workItem = item;
			while(workItem->prev != NULL)
                workItem = workItem->prev;
			while(workItem != NULL)
            {
                workItem->highlighted = false;
                workItem = workItem->next;
            }
			item->highlighted = true;

			Core_->goVerse = true;
			Core_->currentModule = module;

			Core_->currentModule->setKey(Core_->currentVerse);
			Core_->currentVerse.copyFrom(Core_->currentModule->getKey());

			Core_->getContentsWidget()->update();
		}
		break;
	}
}

/*   sbList::onTapped
 *   input
 */
void sbList::onTapped(int x, int y)
{
	sbItem *item = getItem(x, y);

	if(item == NULL)
        return;

	if(item->type == sbItem::TYPE_VERSE)
	{
        ;
	}
	else if(item->type == sbItem::TYPE_BUTTON_SELECTION_BOOK || item->type == sbItem::TYPE_BUTTON_SELECTION_CHAPTER)
	{
        sword::VerseKey workKey = Core_->currentModule == NULL ? Core_->defaultModule->getKey() : Core_->currentModule->getKey();

        if(Core_->searchRange.Count() != 1)
            Core_->searchRange = workKey.ParseVerseList("Gen-Rev",NULL, true, true);

        workKey.Index(item->index);

        switch(Menu::execMenu(QStringList() << "Search From" << "Search To"))
        {
        case 0:
            ((sword::VerseKey*)Core_->searchRange.getElement(0))->LowerBound(workKey);
            break;
        case 1:
            ((sword::VerseKey*)Core_->searchRange.getElement(0))->UpperBound(workKey);
            break;
        }
	}
    else if(item->type == sbItem::TYPE_BUTTON_SELECTION_MODULE)
    {
        CSwordModuleInfo *module = CSwordBackend::instance()->findSwordModuleByPointer(\
            reinterpret_cast<const sword::SWModule*>(item->index));

        switch(Menu::execMenu(QStringList() << "Set as Default" << "Hide" << "Unhide All" << "Remove Module"))
        {
        case 0:
            switch(module->category())
            {
            case CSwordModuleInfo::Bible:
                CBTConfig::set(CBTConfig::standardBible, module);
                break;
            case CSwordModuleInfo::Commentary:
                CBTConfig::set(CBTConfig::standardCommentary, module);
                break;
            }
            break;
        }
    }
}


/*   sbList::process
 *   fill verses with text from module
 *   also delete unnecessary items
 */
void sbList::process ( bool * abort )
{
	Q_ASSERT(type == View::TYPE_MODULE_VIEW);
	Q_ASSERT(_module != NULL);
	sword::SWModule *mod = Core_->getSword()->getModule(_module);
	Q_ASSERT(mod != NULL);
	sword::VerseKey workKey(mod->getKey());

	CSwordModuleInfo *moduleInfo = CSwordBackend::instance()->findModuleByName(_module);
	Q_ASSERT(moduleInfo);
	QList<const CSwordModuleInfo*> moduleList;
	moduleList << moduleInfo;
	
	if(itemCurrent == NULL) return;

	while(!*abort)
	{
		int displace = static_cast<int>(sbList::displace);
		sbItem *item = itemCurrent;
		bool atEnd = false;
		
		// delete unnecessary items, it should be in background thread ...
		if(before.verseCount + after.verseCount > Core_->versesOptimal)
		{
			int countDeleted = 0;
			int needDelete = (before.verseCount + after.verseCount) / Core_->versesOptimal * 10;

			if(after.verseCount > before.verseCount)
			{
				while(countDeleted < needDelete)
				{
					after.verse = after.verse->prev;
					while(after.verse->type != sbItem::TYPE_VERSE)
                        after.verse = after.verse->prev;
					while(after.verse->next != NULL)
					{
						DrawCache.remove(after.verse->next);
						delete after.verse->next;
					}
					countDeleted++;
				}
				if(after.constructed)
                    after.constructed = false;
				after.verseCount -= countDeleted;
			}
			else
			{
				while(countDeleted < needDelete)
				{
					before.verse = before.verse->next;
					while(before.verse->type != sbItem::TYPE_VERSE)
                        before.verse = before.verse->next;
					while(before.verse->prev != NULL)
					{
						DrawCache.remove(before.verse->prev);
						delete before.verse->prev;
					}
					countDeleted++;
				}
				if(before.constructed)
                    before.constructed = false;
				before.verseCount -= countDeleted;
			}
		}

		// find center screen item
		while(item->next != NULL && displace+item->size().height() < (rect.height())/2)
		{
			displace += item->size().height();
			item = item->next;
		}
		
		sbItem *last = item;
		sbItem *first = item;

		while(!*abort)
		{
			if(item->type == sbItem::TYPE_VERSE && item->processed == false)
			{
				DrawCache.remove(item);
				workKey.Index(item->index);
				mod->SetKey(workKey);

				QString text;

				//QString text = QString::fromUtf16(reinterpret_cast<const ushort*>(mod->RenderText()));
				//QString text = QString::fromUtf8(mod->RenderText());

				
				Rendering::CEntryDisplay *display = moduleInfo->getDisplay();
				if(display)
                    text = display->text(moduleList, QString::fromUtf8(workKey.getText()), d_ptr->_displayOptions, \
						d_ptr->_filterOptions);

				if(!text.isEmpty())
                {
				    int oldHeight = item->size().height();

				    item->setText(text);

				    item->expanding = item->size().height();
				    item->setHeight(oldHeight);

				    item->processed = true;

				    return;
                }
			}

			if (atEnd && last->next != NULL)
			{
				last      = last->next;
				item      = last;
				displace += item->size().height();
			}
			else if (first->prev != NULL)
			{
				first     = first->prev;
				item      = first;
				displace -= item->size().height();
			}
			else
				return;

			atEnd = ! atEnd;
		}
	}
}

void sbList::onUpdate()
{
	Q_D(sbList);

    rect = Core_->getContentsWidget()->rect();

    // Change items width if resized
    if(itemCurrent)
    {
        sbItem *i = itemCurrent;
        while(i->prev != 0)
            i = i->prev;
        for(sbItem *i = itemCurrent; i != 0; i = i->next)
        {
            int w = 0;
            for(sbItem *ii = i; ii != 0; ii = ii->right)
                w += ii->size().width();
            if(w != rect.width())
            {
                float f = rect.width()/(float)w;
                for(sbItem *ii = i; ii != 0; ii = ii->right)
                {
                    const int a = ii->size().width() * f - ii->size().width();
                    ii->setWidth(ii->size().width() + a);
                    w += a;
                }
                if(w != rect.width())
                    i->setWidth(i->size().width() + (rect.width() - w));

                Core_->getContentsWidget()->update();
                break;
            }
        }
    }

	// Expand items heights
	if(itemCurrent != NULL && type == View::TYPE_MODULE_VIEW)
	{
		const int halfscreen = rect.height()/2;

		for(int direction = 0; direction < 2; ++direction)
		{
			sbItem *item = itemCurrent;
			float y = displace;

			while(item != NULL)
			{
				if(item->expanding != 0 && !(direction == 1 && item == itemCurrent))
				{
					// out of screen
					if(y < -(item->size().height()+d->_separatorHeight) || y > rect.height())
					{
						item->setHeight(item->expanding);
						item->expanding = 0;
					}
					else
					{
						const int add = qMin((int)qCeil((item->expanding-item->size().height())*0.2f), item->expanding-item->size().height());

                        //qDebug() << "add heights " << item << add;

						item->setHeight(item->size().height()+add);

						if(item->size().height() >= item->expanding)
						{
							item->setHeight(item->expanding);
							item->expanding = 0;
						}

						if(y < halfscreen)
						{
                            float f = static_cast<float>(add);

                            if(y + item->size().height() > halfscreen)
                            	f *= qMin(1.0f, (halfscreen-y)/static_cast<float>(halfscreen));

                            //if(y + item->size().height() > halfscreen)
                            //	f *= (halfscreen-y)/item->size().height();

                            //qDebug() << "y, tem->size().height(), add, f : " << y << item->size().height() << add << f;

                            scroll(-f);
                            y -= f;
                        }
                        Core_->getContentsWidget()->update();
					}
				}

				if ( direction > 0 )
				{
					y += item->size().height();
					item = item->next;
				}
				else
				{
					item = item->prev;
					if(item != NULL)
						y -= item->size().height();
				}
			}
		}
	}

	// add blank verses
	if(type == View::TYPE_MODULE_VIEW && _module != NULL)
	{
		sword::VerseKey workKey(_place);
		int added = 0;

        const char *verseTemplate = "<div class=\"item\"><table><center><b>%1</b></center></table></div>";
        const char *chapterTemplate = "<div class=\"item\"><table><big><b><center>%1 %2</center></b></big></table></div>";

		if(itemCurrent == NULL)
		{
			itemCurrent = new sbItem(sbItem::TYPE_VERSE, QString(verseTemplate).arg(workKey.getVerse()), rect.width(), workKey.Index());

			before.verse = itemCurrent;
			after.verse = itemCurrent;

			if(workKey.getVerse() == 1)
				itemCurrent->attach(new sbItem(sbItem::LOOK_CAPTION, QString(chapterTemplate).arg(QString::fromUtf8(workKey.getBookName())).arg(workKey.getChapter()), rect.width(), workKey.Index()), DIRECTION_PREV);

			added++;
			needFill = true;
		}

		while(true)
		{
			bool atEnd;

			// need for memory reason
			if(after.verseCount+before.verseCount > Core_->versesMax)
				break;
			if(after.verseCount+before.verseCount > Core_->versesOptimal * 2)
				break;

			if(after.verseCount < 15 && !after.constructed)
                atEnd = true;
			else if(before.verseCount < 5 && !before.constructed)
                atEnd = false;
			else if(added > 4)
                break;
			else
			{
				if(before.verseCount+after.verseCount > Core_->versesOptimal/2)
                    break;

                atEnd = added % 2 == 0;

				if((after.constructed && atEnd) || (before.constructed && !atEnd))
				{
					added++;
					continue;
				}
			}

			sbItem *item = atEnd ? after.verse : before.verse;
			int index = 0;

			while(true)
			{
				if(item->type == sbItem::TYPE_VERSE)
                    index = item->index;
				if(atEnd ? item->next == NULL : item->prev == NULL)
                    break;

				item = atEnd ? item->next : item->prev;
			}

			workKey.Index(index);
			atEnd ? workKey.increment() : workKey.decrement();

			if(workKey.Index() != index)
			{
				item->attach(new sbItem(sbItem::TYPE_VERSE, QString(verseTemplate).arg(workKey.getVerse()), rect.width(), workKey.Index()), atEnd ? DIRECTION_NEXT : DIRECTION_PREV);

				if(atEnd)
				{
					after.verse = item->next;
					after.verseCount++;
				}
				else
				{
					before.verse = item->prev;
					before.verseCount++;
				}

				if(workKey.getVerse() == 1)
				{
					QString str(chapterTemplate);
                    str = str.arg(QString::fromUtf8(workKey.getBookName())).arg(workKey.getChapter());

					if(atEnd)
						after.verse->attach(new sbItem(sbItem::LOOK_CAPTION, str, rect.width(), workKey.Index()), DIRECTION_PREV);
					else
						before.verse->attach(new sbItem(sbItem::LOOK_CAPTION, str, rect.width(), workKey.Index()), DIRECTION_PREV);
				}
				added++;
			}
			else
			{
				atEnd ? after.constructed = true : before.constructed = true;
			}
		}
	}
}

/*  sbList::onActivate
 *  called before first redraw
 */
void sbList::onActivate()
{
	Q_D(sbList);
	itemSelected = NULL;
	switch(type)
	{
	case View::TYPE_SEARCH:
		{
			QLabel *l = Core_->getHeaderWidget()->findChild<QLabel*>("search: label");
			if(l)
			{
				if (Core_->currentModule != NULL)
				{
					if(!Core_->searchRange.Count())
					{
						sword::VerseKey workKey = Core_->currentModule->getKey();
						Core_->searchRange = workKey.ParseVerseList("Gen-Rev",NULL,true,true);
					}
					l->	setText(QString("Search %1 in range: %2").arg(Core_->currentModule->Name()).arg(QString::fromUtf8(Core_->searchRange.getRangeText())));
				}
				else
					l->	setText("No Module Selected");
			}
		}
		break;

	case View::TYPE_MODULE_INSTALLER_DISCLAIMER:
		{
			clear();
			itemCurrent = new sbItem (sbItem::DO_NONE, QString("WARNING\n \nAlthough Install Manager provides a convenient way for installing and upgrading SWORD components, it also uses a systematic method for accessing sites which gives packet sniffers a target to lock into for singling out users.\n \nIF YOU LIVE IN A PERSECUTED COUNTRY AND DO NOT WISH TO RISK DETECTION, YOU SHOULD <b>NOT</b> USE INSTALL MANAGER'S REMOTE SOURCE FEATURES.\n \nAlso, Remote Sources other than CrossWire may contain less than quality modules, modules with unorthodox content, or even modules which are not legitimately distributable.  Many repositories contain wonderfully useful content.  These repositories simply are not reviewed or maintained by CrossWire and CrossWire cannot be held responsible for their content.\n \nIf you understand this and are willing to enable remote source features then type yes at the prompt."),rect.width());
		}
		break;

	case View::TYPE_MODULE_INSTALLER:
		{
            Core_->getInstallMgr()->setUserDisclaimerConfirmed(true);

			sbItem *tempItem;
			bool refreshed = atoi((*Core_->getInstallMgr()->installConf)["General"].getWithDefault("Refreshed","0"))  == 0 ? false : true;

			clear();

			int success;
			if(!refreshed)
			    success = Core_->getInstallMgr()->refreshRemoteSourceConfiguration();

			if(success != 0)
			    qDebug("Install Manager Refresh Sources : %i\n", success);

			tempItem = itemCurrent = sbItem::create(sbItem::LOOK_SEPARATOR, QString("Available repositories:"),rect.width());
			
			for(sword::InstallSourceMap::iterator it = Core_->getInstallMgr()->sources.begin(); it != Core_->getInstallMgr()->sources.end(); it++)
			{
				tempItem->attach(sbItem::create(sbItem::TYPE_MODULE_INSTALLER_REPO, QString("<b><center>%1</center></b>").arg((*it).second->caption.c_str()),rect.width(), (long)(*it).second), DIRECTION_NEXT);
				tempItem = tempItem->next;

				if(!refreshed)
				    Core_->getInstallMgr()->refreshRemoteSource((*it).second);

				Core_->getInstallMgr()->getModuleStatus(*Core_->getSword(),*(*it).second->getMgr());
			}

			if (Core_->getInstallMgr()->sources.size() > 0)
				(*Core_->getInstallMgr()->installConf)["General"]["Refreshed"] = "1";
		}
		break;

	case View::TYPE_MENU_OPTIONS:
		{
			clear();

			itemCurrent = new sbItem(sbItem::LOOK_SEPARATOR, QString("Module Options :"),rect.width());

			for(int i = 0; i < Core_->moduleOptions.size(); ++i)
			{
				sbItem *item = new sbItem(sbItem::TYPE_SWITCH_MODULE_OPTION, QString("%1 : %2").arg(Core_->moduleOptions[i]).arg(Core_->getSword()->getGlobalOption(Core_->moduleOptions[i])),rect.width(),i);

				itemCurrent->attach(item,DIRECTION_NEXT);
				itemCurrent = itemCurrent->next;
			}

			while(itemCurrent->prev != NULL)
			    itemCurrent = itemCurrent->prev;

			Core_->changedGlobalOptions = true;
		}
		break;

	case View::TYPE_HOME:
		{
			clear();

            itemCurrent = new sbItem(sbItem::DO_NONE, QString("<div class=\"item\"><center><h3>Welcome to</h3><h2>BibleTime Mini!</h2></center></div>"));
			
			if(Core_->defaultModule != NULL)
			{
				sword::VerseKey workKey(Core_->defaultModule->getKey());

				if(Core_->currentModule != NULL)
				{
					itemCurrent->attach(new sbItem (sbItem::TYPE_GOTO_MODULES, QString("<div class=\"item\"><center><br/>Return to %1<br/></center></div>").arg(QString::fromUtf8(Core_->currentVerse.getText())), rect.width()), DIRECTION_NEXT);
					itemCurrent = itemCurrent->next;
				}
				else if(Core_->features.history.size() > 0)
				{
                    sword::SWModule *module = Core_->getSword()->getModule(Core_->features.history.back().module);
                    if(module != NULL)
                    {
					    workKey.copyFrom(module->getKey());
					    workKey.Index(Core_->features.history.back().placeEnd);
                        itemCurrent->attach(new sbItem (sbItem::TYPE_GO_HISTORY, QString("<div class=\"item\"><center><br/>Continue read from %1<br/><center></div>").arg(QString::fromUtf8(workKey.getText())), rect.width(), Core_->features.history.size()-1), DIRECTION_NEXT);
					    itemCurrent = itemCurrent->next;
                    }
				}
                if(itemCurrent->type == sbItem::DO_NONE)
                {
                    workKey.setText("John 1:1");
                    itemCurrent->attach(new sbItem (sbItem::TYPE_BUTTON_BOOKMARK, QString("<div class=\"item\"><center><br/>Read Bible ...<br/></center></div>"), rect.width(), workKey.Index()), DIRECTION_NEXT);
                    itemCurrent = itemCurrent->next;
                }
			}
			else
			{
				itemCurrent->attach(new sbItem (sbItem::DO_NONE, QString("No modules Found!\n\nPlease download any Bible module from www.crosswire.org and put \"mods.d\" and \"modules\" folders into application folder."), rect.width()), DIRECTION_NEXT);
				itemCurrent = itemCurrent->next;
				itemCurrent->attach(new sbItem (sbItem::TYPE_OPEN_MODULE_INSTALLER, QString("<b><center>Install Modules ...</center></b>"), rect.width()),DIRECTION_NEXT);
				itemCurrent = itemCurrent->next;
            }
            itemCurrent->attach(new sbItem (sbItem::DO_NONE, QString("<div class=\"item\"><small><center>About :<br/>"
                "Built on %1<br/>Using versions: Sword %2, Qt %3, BibleTime %4<br/>Send your feedback to kalemas@mail.ru</center></small></div>" \
                ).arg(__DATE__).arg(sword::SWVersion::currentVersion.getText()).arg(QT_VERSION_STR).arg(BT_VERSION), rect.width()), DIRECTION_NEXT);
			itemCurrent = itemCurrent->next;

			while(itemCurrent->prev != NULL)
				itemCurrent = itemCurrent->prev;
		}
		break;

	case View::TYPE_MODULE_VIEW:
		{
			if(Core_->goVerse)
			{
				Core_->goVerse = false;

				if(Core_->currentModule == NULL)
                    Core_->currentModule = Core_->defaultModule;

				_module = Core_->holdString(Core_->currentModule->Name());

				clear();

				_place.copyFrom(Core_->currentVerse);

				if(itemBanner)
				{
					DrawCache.remove(itemBanner);
					delete itemBanner;
					itemBanner = NULL;
				}

				displace = rect.height()*2.0f/5.0f;

				// adding blank verses starts automatically by timer event
				// filling with text occurs in sbList::process in another thread

				// update history
				attachedHistory = Core_->features.addHistory(_module, _place.Index());

				Core_->getContentsWidget()->update();

				//if(options.size() != Core_->moduleOptions.size())
				//{
				//	options.resize(Core_->moduleOptions.size());
				//	for(int i = 0; i<Core_->moduleOptions.size(); ++i)
				//	{
				//		options[i] = Core_->holdString(Core_->getSword()->getGlobalOption(Core_->moduleOptions[i]));
				//	}
				//}
			}

			if(_module != NULL)
			{
				if(next == NULL)
				    attach(new sbList(View::TYPE_MODULE_VIEW), View::PIN_NEXT);
				if(prev == NULL)
				    attach(new sbList(View::TYPE_MODULE_VIEW), View::PIN_PREV);

				// if global options changed , reprocess items text
				//bool needReprocess = false;

				//if(!Core_->changedGlobalOptions)
				//{
				//	for(int i = 0; i < options.size(); ++i)
				//		Core_->getSword()->setGlobalOption(Core_->moduleOptions[i],options[i]);
				//}

				//for(int i = 0; i < Core_->moduleOptions.size(); ++i)
				//{
				//	if(strcmp(Core_->getSword()->getGlobalOption(Core_->moduleOptions[i]), options[i]))
				//	{
				//		needReprocess = true;
				//		options[i] = Core_->holdString(Core_->getSword()->getGlobalOption(Core_->moduleOptions[i]));
				//	}
				//}

				//if(needReprocess)
				//{
				//	for (sbItem *item = itemCurrent; item != NULL; item = item->next)
				//	    item->processed = false;
				//	for (sbItem *item = itemCurrent; item != NULL; item = item->prev)
				//	    item->processed = false;
				//}

				Core_->currentModule = Core_->getSword()->getModule(_module);
				Core_->currentVerse.copyFrom(_place);
				Core_->changedGlobalOptions = false;

			}

            // update header
            QLabel *l = Core_->getHeaderWidget()->findChild<QLabel*>("module: label");
            if(l && _module)
                l->setText(QString("%1: %2").arg(_module).arg(_place.getShortText()));
            else if (l)
                l->setText("");
            QPushButton *b1 = Core_->getHeaderWidget()->findChild<QPushButton*>("arrow left");
            QPushButton *b2 = Core_->getHeaderWidget()->findChild<QPushButton*>("arrow right");
            if(b1)
                b1->setEnabled(prev != 0);
            if(b2)
                b2->setEnabled(next != 0);
		}
		break;

	case View::TYPE_SELECT_BOOK:
		{
			sbItem *setItem = NULL;
			int t = 0;

			sword::VerseKey workKey(Core_->currentVerse);

			const char *moduleBook = workKey.getBookName();

			clear();

			workKey.setPosition(sword::TOP);

			while(true)
			{
			    sbItem *item = new sbItem (sbItem::TYPE_BUTTON_SELECTION_BOOK, QString("<div class=\"item\"><h3><center>%1</center></h3></div>").arg(QString::fromUtf8(workKey.getBookName())), rect.width(), workKey.Index());

				if (moduleBook == workKey.getBookName())
				    setItem = item;

				if(itemCurrent == NULL)
				{
					itemCurrent = item;
				}
				else
				{
					itemCurrent->attach(item, DIRECTION_NEXT);
					itemCurrent = itemCurrent->next;
				}

				// testament separator
				if(t != workKey.getTestament())
				{
					if(t == 0)
					    itemCurrent->attach(sbItem::create(sbItem::LOOK_SEPARATOR, QString("<small><center>Old Testament</center></small>"),rect.width()), DIRECTION_PREV);
					else if(t == 1)
					    itemCurrent->attach(sbItem::create(sbItem::LOOK_SEPARATOR, QString("<small><center>New Testament</center></small>"),rect.width()), DIRECTION_PREV);

					t = workKey.getTestament();
				}

				// next book
				int b = workKey.getBook();

				workKey.setBook(workKey.getBook()+1);

				if(b == workKey.getBook())
				    break;
			}

			if(setItem != NULL)
			{
				if(setItem->prev != NULL && setItem->prev->type == sbItem::LOOK_SEPARATOR)
					itemCurrent = setItem->prev;
				else
					itemCurrent = setItem;
			}
			else 
				while (itemCurrent->prev != NULL)
				    itemCurrent = itemCurrent->prev;
		}
		break;

	case View::TYPE_SELECT_MODULE:
		{
			clear();

			sword::StringList sl;
			sbItem * tempItem;

			for(sword::ModMap::iterator it = Core_->getSword()->Modules.begin(); it != Core_->getSword()->Modules.end(); it++)
			{
				if(std::find(sl.begin(),sl.end(),(*it).second->Type()) == sl.end())
				{
					sl.push_back((*it).second->Type());
					tempItem = sbItem::create(sbItem::LOOK_SEPARATOR, QString((*it).second->Type()),rect.width());

					if(itemCurrent == NULL)
						itemCurrent = tempItem;
					else
						itemCurrent->attach(tempItem,DIRECTION_NEXT), itemCurrent = itemCurrent->next;
					
					for(sword::ModMap::iterator im = it; im != Core_->getSword()->Modules.end(); im++)
					{
						if(!strcmp((*it).second->Type(),(*im).second->Type()))
						{
							itemCurrent->attach(new sbItem (sbItem::TYPE_BUTTON_SELECTION_MODULE, QString("<b>%1</b><br/><small>%2</small>").arg((*im).second->Name()).arg((*im).second->Description()), rect.width(), (long)(*im).second), DIRECTION_NEXT);
							itemCurrent = itemCurrent->next;

							if(Core_->currentModule == (*im).second)
							    itemCurrent->highlighted = true;
						}
					}
				}
			}
			tempItem = sbItem::create (sbItem::TYPE_OPEN_MODULE_INSTALLER, QString("<b><center>Install Modules ...</center></b>"), rect.width());

			itemCurrent == NULL ? itemCurrent = tempItem : itemCurrent->attach(tempItem, DIRECTION_NEXT);

			while(itemCurrent->prev != NULL)
			    itemCurrent = itemCurrent->prev;
		}
		break;

	case View::TYPE_HISTORY:
		{
			bool createdNow = false;
			bool createdToday = false;
			bool createdYesterday = false;
			bool createdWeek = false;
			bool createdOlder = false;

			QDateTime startedToday     (QDate::currentDate());
			QDateTime startedYesterday (startedToday.addDays(-1));
			QDateTime startedWeek      (startedToday.addDays(-7));

			clear();
			
			for (int h = Core_->features.history.size()-1; h >= 0 && Core_->features.history.size()-h <= 50 ; h--)
			{
				sbFeatures::History * hi = &Core_->features.history[h];

				sword::VerseKey workKey ( Core_->getSword()->getModule(hi->module)->getKey() );
				workKey.Index (hi->placeEnd);

				sbItem * timeHeader = NULL;

				if (hi->visitStart > Core_->systemStarted_)
				{
					if(!createdNow)
					{
						timeHeader = new sbItem (sbItem::LOOK_SEPARATOR, QString("<small><center>Now</center></small>"), rect.width());
						createdNow = true;
					}
				}
				else if (hi->visitStart > startedToday.toMSecsSinceEpoch())
				{
					if(!createdToday)
					{
						timeHeader = new sbItem (sbItem::LOOK_SEPARATOR, QString("<small><center>Today</center></small>"), rect.width());
						createdToday = true;
					}
				}
				else if (hi->visitStart > startedYesterday.toMSecsSinceEpoch())
				{
					if(!createdYesterday)
					{
						timeHeader = new sbItem (sbItem::LOOK_SEPARATOR, QString("<small><center>Yesterday</center></small>"), rect.width());
						createdYesterday = true;
					}
				}
				else if (hi->visitStart > startedWeek.toMSecsSinceEpoch())
				{
					if(!createdWeek)
					{
						timeHeader = new sbItem (sbItem::LOOK_SEPARATOR, QString("<small><center>Last Week</center></small>"), rect.width());
						createdWeek = true;
					}
				}
				else if (!createdOlder)
				{
					timeHeader = new sbItem (sbItem::LOOK_SEPARATOR, QString("<small><center>Older</center></small>"), rect.width());
					createdOlder = true;
				}

				if (timeHeader != NULL)
				{
					if ( itemCurrent == NULL )
					{
						itemCurrent = timeHeader;
					}
					else
					{
						itemCurrent->attach(timeHeader,DIRECTION_NEXT);
						itemCurrent = itemCurrent->next;
					}
				}
				
                sbItem *item = new sbItem (sbItem::TYPE_HISTORY, QString("<table width=100%><tr><td align=left>%1<br/><small>%4</small></td><td align=right><big><b>%2:%3</b></big></td></tr></table>").arg(QString::fromUtf8(workKey.getBookName())).arg(workKey.getChapter()).arg(workKey.getVerse()).arg(hi->module), rect.width(), h);

				if(itemCurrent == NULL)
				{
					itemCurrent = item;
				}
				else
				{
					itemCurrent->attach(item, DIRECTION_NEXT);
					itemCurrent = itemCurrent->next;
				}
			}

			if ( itemCurrent != NULL ) while ( itemCurrent->prev != NULL ) itemCurrent = itemCurrent->prev;
		}
		break;

	default:
		;
	}

	// activate widgets
	foreach(QObject *obj, Core_->getControlsWidget()->children() + Core_->getHeaderWidget()->children())
	{
		QWidget *w = qobject_cast<QWidget*>(obj);
		if(w)
			w->hide();
	}
    foreach(QString str, d_ptr->_controls)
    {
        QWidget *w = Core_->getControlsWidget()->findChild<QWidget*>(str);
        if(w)
            w->show();
        else
            qDebug() << "Control activation failed for" << type << str;
	}
	foreach(QString str, d_ptr->_indicators)
	{
		QWidget *w = Core_->getHeaderWidget()->findChild<QWidget*>(str);
		if(w)
			w->show();
		else
			qDebug() << "Indicator activation failed for" << type << str;
	}

	Core_->getContentsWidget()->update();

    // show tips
    if(type == View::TYPE_MODULE_VIEW && CBTConfig::get(CBTConfig::showTipAboutModules))
    {
        QScopedPointer<Menu> dialog(new Menu);
        QVBoxLayout *v = new QVBoxLayout;

        QLabel *l = new QLabel("Here are texts. You can scroll up/down to read text and "
            "slide left/right to switch texts. By sliding active view left or right, you "
            "will get an empty view, where you can create, for example, commentary for "
            "verse from Bible view. Current verse and module selection and also search is "
            "accessible by Goto button.", dialog.data());

        l->setWordWrap(true);
        l->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

        QFont f = l->font();
        f.setPixelSize(f.pixelSize() * 0.75);
        f.setBold(false);
        l->setFont(f);

        QCheckBox *cb = new QCheckBox("show again", dialog.data());

        cb->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        cb->setChecked(true);

        f = cb->font();
        f.setPixelSize(f.pixelSize() * 0.75);
        f.setBold(true);
        cb->setFont(f);

        v->addWidget(l, 0, Qt::AlignCenter);
        v->addWidget(cb, 0, Qt::AlignCenter);

        dialog->setLayout(v);

        dialog->exec();

        if(!cb->isChecked())
            CBTConfig::set(CBTConfig::showTipAboutModules, false);
    }
}

bool sbList::onKey ( int key )
{
	switch ( key )
	{
	case Qt::Key_Return:
		if (itemSelected != NULL)
		{
			onItem(itemSelected);
			Core_->getContentsWidget()->update();
		}
		break;

	case Qt::Key_Left:
		if (itemSelected == NULL && type == View::TYPE_MODULE_VIEW)
		{
            Core_->switchList(View::PIN_PREV);
		}
		else
		{
			if (itemSelected != NULL && itemSelected->left != NULL)
			{
				itemSelected = itemSelected->left;
			}

			Core_->getContentsWidget()->update();
		}
		return true;

	case Qt::Key_Right:
		if (itemSelected == NULL && type == View::TYPE_MODULE_VIEW)
		{
            Core_->switchList(View::PIN_NEXT);
		}
		else
		{
			if (itemSelected != NULL && itemSelected->right != NULL)
			{
				itemSelected = itemSelected->right;
			}
			Core_->getContentsWidget()->update();
		}
		break;

	case Qt::Key_Down:
		if ( itemSelected == NULL )
			itemSelected = itemCurrent;
		else if ( itemSelected->next != NULL )
			itemSelected = itemSelected->next;
		Core_->getContentsWidget()->update();
		break;

	case Qt::Key_Up:
		if ( itemSelected == NULL )
			itemSelected = itemCurrent;
		else if ( itemSelected->prev != NULL )
			itemSelected = itemSelected->prev;
		Core_->getContentsWidget()->update();
		break;

	case Qt::Key_Escape:
		QApplication::exit(1);
		break;

	default:
		return false;
	}

	return true;
}

void sbList::performSearch(QString request)
{
    CSwordModuleSearch searcher;

    CSwordModuleInfo *m = CSwordBackend::instance()->findSwordModuleByPointer(Core_->currentModule);

    if(!m->hasIndex())
    {
        if(Menu::execQuery("Build index for module?", QStringList() << "Yes" << "No") != 0)
            return;

        QScopedPointer<Menu> dialog(Menu::createProgress("Indexing..."));

        QObject::connect(dialog.data(), SIGNAL(canceled()), m, SLOT(cancelIndexing()));
        QObject::connect(m, SIGNAL(indexingFinished()), dialog.data(), SLOT(finish()));
        QObject::connect(m, SIGNAL(indexingProgress(int)), dialog.data(), SLOT(setPercent(int)));

        dialog->show();

        if(!m->buildIndex() || dialog->wasCanceled())
        {
            if (m->hasIndex())
                m->deleteIndex();
            return;
        }
    }

    // Set the search options:
    searcher.setSearchedText(request);
    searcher.setModules(QList<const CSwordModuleInfo*>() << m);
    searcher.setSearchScope(Core_->searchRange);
    
    searcher.startSearch();

    clear();

    // Display the search results:
    foreach(sword::ListKey r, searcher.results())
    {
        for(int i=0; i < r.Count(); ++i)
        {
            sbItem *item = new sbItem (sbItem::TYPE_BUTTON_BOOKMARK, QString::fromUtf8(r.GetElement(i)->getText()), rect.width(), r.GetElement(i)->Index());
    	    if(itemCurrent)
    	    {
    		    itemCurrent->attach(item, DIRECTION_NEXT);
    		    itemCurrent = itemCurrent->next;
    	    }
    	    else
    		    itemCurrent = item;
        }
    }
    while(itemCurrent && itemCurrent->prev)
    	itemCurrent = itemCurrent->prev;

    Core_->getContentsWidget()->update();

	//if(request.isEmpty())
	//	return;

	//qDebug() << "Start search" << request;

	//QProgressDialog *dialog = new QProgressDialog(Core_->getMainWidget());

	//dialog->setMaximum(100);
	//dialog->setAutoClose(false);
	//dialog->setCancelButtonText("Cancel");
	//dialog->setLabelText(QString("Search for %1").arg(request));
	//dialog->show();
	//dialog->repaint();

	//Core_->currentModule->createSearchFramework();
	//
	//sword::ListKey result = Core_->currentModule->search(request.toUtf8(), 0, 0, \
    //  &Core_->searchRange, 0, &sbListPrivate::searchProgress, dialog);
	//
	//Core_->currentModule->deleteSearchFramework();

	//delete dialog;

	//clear();
	//for(int i=0; i < result.Count(); ++i)
	//{
	//	sbItem *item = new sbItem (sbItem::TYPE_BUTTON_BOOKMARK, QString::fromUtf8(result.GetElement(i)->getText()), rect.width(), result.GetElement(i)->Index());
	//	if(itemCurrent)
	//	{
	//		itemCurrent->attach(item, DIRECTION_NEXT);
	//		itemCurrent = itemCurrent->next;
	//	}
	//	else
	//		itemCurrent = item;
	//}
	//while(itemCurrent && itemCurrent->prev)
	//	itemCurrent = itemCurrent->prev;

    //Core_->getContentsWidget()->update();
}
