#pragma once

/*
 * Qt Advanced Docking System
 * Copyright (C) 2017 Uwe Kindler
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; If not, see <http://www.gnu.org/licenses/>.
 */

/// @file   CDockWidgetTab.h
/// @author Uwe Kindler
/// @date   27.02.2017
/// @brief  Declaration of CDockWidgetTab class.

#include <QFrame>
#include <QSize>

#include "ads_globals.h"

QT_FORWARD_DECLARE_CLASS(QMenu)

namespace ads
{
class CDockWidget;
class CDockAreaWidget;
struct DockWidgetTabPrivate;
class CDockManager;

/**
 * @brief A dock widget tab that shows a title and an icon.
 * The dock widget tab is shown in the dock area title bar to switch between
 * tabbed dock widgets.
 */
class ADS_EXPORT CDockWidgetTab : public QFrame
{
		Q_OBJECT
		Q_PROPERTY(bool activeTab READ isActiveTab WRITE setActiveTab NOTIFY activeTabChanged)
		Q_PROPERTY(QSize iconSize READ iconSize WRITE setIconSize)

	public:
		/**
		 * @brief Default Constructor
		 * param[in] DockWidget The dock widget this title bar belongs to
		 * param[in] parent The parent widget of this title bar.
		 */
		CDockWidgetTab(CDockWidget* DockWidget, QWidget* parent = nullptr);

		/**
		 * @brief Virtual Destructor.
		 */
		~CDockWidgetTab() override;

	private:
		/**
		 * @brief Slot invoked on dock widget features changed.
		 */
		void onDockWidgetFeaturesChanged();

	private Q_SLOTS:
		/**
		 * @brief Detaches the dock widget into a floating widget.
		 */
		void detachDockWidget();
		/**
		 * @brief Moves the dock widget into the auto hide area.
		 */
		void autoHideDockWidget();
		/**
		 * @brief Slot invoked on auto hide to action clicked.
		 */
		void onAutoHideToActionClicked();

	protected:
		/**
		 * @brief Handles the mouse press event.
		 */
		void mousePressEvent(QMouseEvent* ev) override;
		/**
		 * @brief Handles the mouse release event.
		 */
		void mouseReleaseEvent(QMouseEvent* ev) override;
		/**
		 * @brief Handles the mouse move event.
		 */
		void mouseMoveEvent(QMouseEvent* ev) override;
		/**
		 * @brief Handles the context menu event.
		 */
		void contextMenuEvent(QContextMenuEvent* ev) override;

		/**
		 * @brief Double-clicking the tab widget makes the assigned dock widget floating.
		 */
		void mouseDoubleClickEvent(QMouseEvent* event) override;

		/**
		 * @brief Track event ToolTipChange and set child ToolTip.
		 */
		bool event(QEvent* e) override;

	public:
		/**
		 * @brief Alias for the base class.
		 */
		using Super = QFrame;

		/**
		 * @brief Returns true, if this is the active tab.
		 */
		bool isActiveTab() const;

		/**
		 * @brief Set this true to make this tab the active tab.
		 */
		void setActiveTab(bool active);

		/**
		 * @brief Sets the dock area widget the dockWidget returned by dockWidget()
		 * function belongs to.
		 */
		void setDockAreaWidget(CDockAreaWidget* DockArea);

		/**
		 * @brief Returns the dock area widget this title bar belongs to.
		 * @return This function returns 0 if the dock widget that owns this title
		 * bar widget has not been added to any dock area yet.
		 */
		CDockAreaWidget* dockAreaWidget() const;

		/**
		 * @brief Returns the dock widget this title widget belongs to.
		 */
		CDockWidget* dockWidget() const;

		/**
		 * @brief Sets the icon to show in title bar.
		 */
		void setIcon(const QIcon& Icon);

		/**
		 * @brief Returns the icon.
		 */
		const QIcon& icon() const;

		/**
		 * @brief Returns the tab text.
		 */
		QString text() const;

		/**
		 * @brief Sets the tab text.
		 */
		void setText(const QString& title);

		/**
		 * @brief Returns true if text is elided on the tab's title.
		 */
		bool isTitleElided() const;

		/**
		 * @brief This function returns true if the assigned dock widget is closable.
		 */
		bool isClosable() const;

		/**
		 * @brief This function returns true if the assigned dock widget is floatable.
		 */
		bool isFloatable() const;

		/**
		 * @brief Sets the text elide mode.
		 */
		void setElideMode(Qt::TextElideMode mode);

		/**
		 * @brief Update stylesheet style if a property changes.
		 */
		void updateStyle();

		/**
		 * @brief Returns the icon size.
		 * If no explicit icon size has been set, the function returns an invalid
		 * QSize.
		 */
		QSize iconSize() const;

		/**
		 * @brief Set an explicit icon size.
		 * If no icon size has been set explicitly, then the tab sets the icon size
		 * depending on the style.
		 */
		void setIconSize(const QSize& Size);

		/**
		 * @brief Returns the current drag state of this tab.
		 * Use this function to determine if the tab is currently being dragged.
		 */
		eDragState dragState() const;

		/**
		 * @brief Fills the provided menu with standard entries. If a nullptr is passed, a
		 * new menu is created and filled with standard entries.
		 * This function is called from the actual version of contextMenuEvent, but
		 * can be called from any code. Caller is responsible for deleting the created
		 * object.
		 *
		 * @param menu The QMenu to fill with standard entries. If nullptr, a new
		 * QMenu will be created.
		 * @return The filled QMenu, either the provided one or a newly created one if
		 * nullptr was passed.
		 */
		virtual QMenu* buildContextMenu(QMenu* menu);

	public Q_SLOTS:
		/**
		 * @brief Sets the visibility.
		 */
		void setVisible(bool visible) override;

	Q_SIGNALS:
		/**
		 * @brief Signal emitted when active tab changed.
		 */
		void activeTabChanged();
		/**
		 * @brief Signal emitted when clicked.
		 */
		void clicked();
		/**
		 * @brief Signal emitted when close requested.
		 */
		void closeRequested();
		/**
		 * @brief Signal emitted when close other tabs requested.
		 */
		void closeOtherTabsRequested();
		/**
		 * @brief Signal emitted when moved.
		 */
		void moved(const QPoint& GlobalPos);
		/**
		 * @brief Signal emitted when elided changed.
		 */
		void elidedChanged(bool elided);

	private:
		friend struct DockWidgetTabPrivate;
		friend class CDockWidget;
		friend class CDockManager;
		friend class CAutoHideDockContainer;
		/// Private data (pimpl).
		DockWidgetTabPrivate* d;
};
}// namespace ads

// namespace ads
