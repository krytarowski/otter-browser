/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2015 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
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

#ifndef OTTER_QTWEBENGINEWEBWIDGET_H
#define OTTER_QTWEBENGINEWEBWIDGET_H

#include "../../../../ui/WebWidget.h"

#include <QtNetwork/QNetworkReply>
#include <QtWebEngineWidgets/QWebEngineDownloadItem>
#include <QtWebEngineWidgets/QWebEngineView>

namespace Otter
{

class ContentsDialog;

class QtWebEngineWebWidget : public WebWidget
{
	Q_OBJECT

public:
	enum HitTestFlag
	{
		NoFlagsTest = 0,
		IsContentEditableTest = 1,
		IsEmptyTest = 2,
		IsFormTest = 4,
		IsSelectedTest = 8,
		MediaHasControlsTest = 16,
		MediaIsLoopedTest = 32,
		MediaIsMutedTest = 64,
		MediaIsPausedTest = 128
	};

	Q_DECLARE_FLAGS(HitTestFlags, HitTestFlag)

	struct HitTestResult
	{
		QString title;
		QString tagName;
		QString alternateText;
		QString longDescription;
		QUrl formUrl;
		QUrl frameUrl;
		QUrl imageUrl;
		QUrl linkUrl;
		QUrl mediaUrl;
		QRect geometry;
		HitTestFlags flags;

		HitTestResult() : flags(NoFlagsTest) {}

		explicit HitTestResult(const QVariant &result)
		{
			const QVariantMap map = result.toMap();
			const QVariantMap geometryMap = map.value(QLatin1String("geometry")).toMap();

			title = map.value(QLatin1String("title")).toString();
			tagName = map.value(QLatin1String("tagName")).toString();
			alternateText = map.value(QLatin1String("alternateText")).toString();
			longDescription = map.value(QLatin1String("longDescription")).toString();
			formUrl = QUrl(map.value(QLatin1String("formUrl")).toString());
			frameUrl = QUrl(map.value(QLatin1String("frameUrl")).toString());
			imageUrl = QUrl(map.value(QLatin1String("imageUrl")).toString());
			linkUrl = QUrl(map.value(QLatin1String("linkUrl")).toString());
			mediaUrl = QUrl(map.value(QLatin1String("mediaUrl")).toString());
			geometry = QRect(geometryMap.value(QLatin1String("x")).toInt(), geometryMap.value(QLatin1String("y")).toInt(), geometryMap.value(QLatin1String("w")).toInt(), geometryMap.value(QLatin1String("h")).toInt());
			flags = static_cast<HitTestFlags>(map.value(QLatin1String("flags")).toInt());
		}
	};

	void search(const QString &query, const QString &engine);
	void print(QPrinter *printer);
	WebWidget* clone(bool cloneHistory = true);
	Action* getAction(int identifier);
	QString getTitle() const;
	QString getSelectedText() const;
	QUrl getUrl() const;
	QIcon getIcon() const;
	QPixmap getThumbnail();
	QPoint getScrollPosition() const;
	QRect getProgressBarGeometry() const;
	WindowHistoryInformation getHistory() const;
	QHash<QByteArray, QByteArray> getHeaders() const;
	QVariantHash getStatistics() const;
	int getZoom() const;
	bool isLoading() const;
	bool isPrivate() const;
	bool findInPage(const QString &text, FindFlags flags = NoFlagsFind);
	bool eventFilter(QObject *object, QEvent *event);

public slots:
	void clearOptions();
	void clearSelection();
	void goToHistoryIndex(int index);
	void triggerAction(int identifier, bool checked = false);
	void setPermission(const QString &key, const QUrl &url, PermissionPolicies policies);
	void setScrollPosition(const QPoint &position);
	void setHistory(const WindowHistoryInformation &history);
	void setZoom(int zoom);
	void setUrl(const QUrl &url, bool typed = true);

protected:
	explicit QtWebEngineWebWidget(bool isPrivate, WebBackend *backend, ContentsWidget *parent = NULL);

	void focusInEvent(QFocusEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void openUrl(const QUrl &url, OpenHints hints = DefaultOpen);
	void pasteText(const QString &text);
	void handleContextMenu(const QVariant &result);
	void handleCreateSearch(const QVariant &result);
	void handleHitTest(const QVariant &result);
	void handleHotClick(const QVariant &result);
	void handleImageProperties(const QVariant &result);
	void handleScroll(const QVariant &result);
	void handleScrollToAnchor(const QVariant &result);
	void handleToolTip(const QVariant &result);
	void updateOptions(const QUrl &url);
	void setHistory(QDataStream &stream);
	void setOptions(const QVariantHash &options);
	QWebEnginePage* getPage();

protected slots:
	void pageLoadStarted();
	void pageLoadFinished();
	void downloadFile(QWebEngineDownloadItem *item);
	void linkHovered(const QString &link);
	void iconReplyFinished();
	void handleIconChange(const QUrl &url);
	void handleAuthenticationRequired(const QUrl &url, QAuthenticator *authenticator);
	void handleProxyAuthenticationRequired(const QUrl &url, QAuthenticator *authenticator, const QString &proxy);
	void handlePermissionRequest(const QUrl &url, QWebEnginePage::Feature feature);
	void handlePermissionCancel(const QUrl &url, QWebEnginePage::Feature feature);
	void handleWindowCloseRequest();
	void notifyTitleChanged();
	void notifyUrlChanged(const QUrl &url);
	void notifyIconChanged();
	void notifyPermissionRequested(const QUrl &url, QWebEnginePage::Feature feature, bool cancel);
	void updateUndo();
	void updateRedo();
	void updatePageActions(const QUrl &url);
	void updateNavigationActions();
	void updateEditActions();
	void updateLinkActions();
	void updateFrameActions();
	void updateImageActions();
	void updateMediaActions();
	void updateBookmarkActions();
	void showContextMenu(const QPoint &position = QPoint(-1, -1));
	void showHotClickMenu();

private:
	QWebEngineView *m_webView;
	QWidget *m_childWidget;
	QNetworkReply *m_iconReply;
	QIcon m_icon;
	HitTestResult m_hitResult;
	QPoint m_clickPosition;
	QPoint m_scrollPosition;
	QHash<int, Action*> m_actions;
	bool m_ignoreContextMenu;
	bool m_ignoreContextMenuNextTime;
	bool m_isUsingRockerNavigation;
	bool m_isLoading;
	bool m_isTyped;

signals:
	void hitTestResultReady();
	void unlockEventLoop();

friend class QtWebEnginePage;
friend class QtWebEngineWebBackend;
};

}

#endif
