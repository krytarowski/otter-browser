/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2013 - 2015 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
* Copyright (C) 2014 Piotr Wójcik <chocimier@tlen.pl>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

#include "BookmarksManager.h"
#include "SessionsManager.h"

#include <QtCore/QDateTime>

namespace Otter
{

BookmarksManager* BookmarksManager::m_instance = NULL;
BookmarksModel* BookmarksManager::m_model = NULL;

BookmarksManager::BookmarksManager(QObject *parent) : QObject(parent),
	 m_saveTimer(0)
{
}

void BookmarksManager::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_saveTimer)
	{
		killTimer(m_saveTimer);

		m_saveTimer = 0;

		if (m_model)
		{
			m_model->save(SessionsManager::getWritableDataPath("bookmarks.xbel"));
		}
	}
}

void BookmarksManager::createInstance(QObject *parent)
{
	if (!m_instance)
	{
		m_instance = new BookmarksManager(parent);
	}
}

void BookmarksManager::scheduleSave()
{
	if (m_saveTimer == 0)
	{
		m_saveTimer = startTimer(1000);
	}
}

void BookmarksManager::updateVisits(const QUrl &url)
{
	if (!m_model)
	{
		getModel();
	}

	const QUrl adjustedUrl = BookmarksModel::adjustUrl(url);

	if (m_model->hasBookmark(adjustedUrl))
	{
		const QList<BookmarksItem*> bookmarks = m_model->getBookmarks(adjustedUrl);

		for (int i = 0; i < bookmarks.count(); ++i)
		{
			bookmarks.at(i)->setData((bookmarks.at(i)->data(BookmarksModel::VisitsRole).toInt() + 1), BookmarksModel::VisitsRole);
			bookmarks.at(i)->setData(QDateTime::currentDateTime(), BookmarksModel::TimeVisitedRole);
		}
	}
}

void BookmarksManager::removeBookmark(const QUrl &url)
{
	if (!m_model)
	{
		getModel();
	}

	const QUrl adjustedUrl = BookmarksModel::adjustUrl(url);

	if (!hasBookmark(adjustedUrl))
	{
		return;
	}

	const QList<BookmarksItem*> items = m_model->findUrls(adjustedUrl);

	for (int i = 0; i < items.count(); ++i)
	{
		items.at(i)->remove();
	}
}

BookmarksManager* BookmarksManager::getInstance()
{
	return m_instance;
}

BookmarksModel* BookmarksManager::getModel()
{
	if (!m_model && m_instance)
	{
		m_model = new BookmarksModel(SessionsManager::getWritableDataPath(QLatin1String("bookmarks.xbel")), BookmarksModel::BookmarksMode, m_instance);

		connect(m_model, SIGNAL(modelModified()), m_instance, SLOT(scheduleSave()));
	}

	return m_model;
}

BookmarksItem* BookmarksManager::addBookmark(BookmarksModel::BookmarkType type, const QUrl &url, const QString &title, BookmarksItem *parent)
{
	if (!m_model)
	{
		getModel();
	}

	return m_model->addBookmark(type, 0, url, title, parent);
}

BookmarksItem* BookmarksManager::getBookmark(const QString &keyword)
{
	if (!m_model)
	{
		getModel();
	}

	return m_model->getBookmark(keyword);
}

BookmarksItem* BookmarksManager::getBookmark(quint64 identifier)
{
	if (!m_model)
	{
		getModel();
	}

	if (identifier == 0)
	{
		return m_model->getRootItem();
	}

	return m_model->getBookmark(identifier);
}

QStringList BookmarksManager::getKeywords()
{
	if (!m_model)
	{
		getModel();
	}

	return m_model->getKeywords();
}

QList<QUrl> BookmarksManager::getUrls()
{
	if (!m_model)
	{
		getModel();
	}

	return m_model->getUrls();
}

bool BookmarksManager::hasBookmark(const QUrl &url)
{
	if (!m_model)
	{
		getModel();
	}

	return m_model->hasUrl(url);
}

bool BookmarksManager::hasKeyword(const QString &keyword)
{
	if (!m_model)
	{
		getModel();
	}

	return m_model->hasKeyword(keyword);
}

}
