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

/// @file   CDockAreaTitleBar.h
/// @author Uwe Kindler
/// @date   12.10.2018
/// @brief  Declaration of CDockAreaTitleBar class.

#include <QFrame>
#include <QToolButton>

#include "CTitleBarButton.h"
#include "ads_globals.h"

QT_FORWARD_DECLARE_CLASS(QAbstractButton)

namespace ads
{
class CDockAreaTabBar;
class CDockAreaWidget;
struct DockAreaTitleBarPrivate;
class CElidingLabel;
class CDockAreaTitleBar;

/**
 * @brief Title bar of a dock area.
 * The title bar contains a tabbar with all tabs for a dock widget group and
 * with a tabs menu button, a undock button and a close button.
 */
class ADS_EXPORT CDockAreaTitleBar : public QFrame
{
		Q_OBJECT

	public:
		/**
		 * @brief Default Constructor.
		 */
		CDockAreaTitleBar(CDockAreaWidget* parent);

		/**
		 * @brief Virtual Destructor.
		 */
		~CDockAreaTitleBar() override;

	private Q_SLOTS:
		/**
		 * @brief Slot handling: tabs menu about to show.
		 */
		void onTabsMenuAboutToShow();
		/**
		 * @brief Slot invoked on close button clicked.
		 */
		void onCloseButtonClicked();
		/**
		 * @brief Slot invoked on auto hide close action triggered.
		 */
		void onAutoHideCloseActionTriggered();
		/**
		 * @brief Minimizes the auto hide container.
		 */
		void minimizeAutoHideContainer();
		/**
		 * @brief Slot invoked on undock button clicked.
		 */
		void onUndockButtonClicked();
		/**
		 * @brief Slot invoked on tabs menu action triggered.
		 */
		void onTabsMenuActionTriggered(QAction* Action);
		/**
		 * @brief Slot invoked on current tab changed.
		 */
		void onCurrentTabChanged(int Index);
		/**
		 * @brief Slot invoked on auto hide button clicked.
		 */
		void onAutoHideButtonClicked();
		/**
		 * @brief Slot invoked on auto hide dock area action clicked.
		 */
		void onAutoHideDockAreaActionClicked();
		/**
		 * @brief Slot invoked on auto hide to action clicked.
		 */
		void onAutoHideToActionClicked();

	protected:
		/**
		 * @brief Stores mouse position to detect dragging.
		 */
		void mousePressEvent(QMouseEvent* ev) override;

		/**
		 * @brief Stores mouse position to detect dragging.
		 */
		void mouseReleaseEvent(QMouseEvent* ev) override;

		/**
		 * @brief Starts floating the complete docking area including all dock widgets,
		 * if it is not the last dock area in a floating widget.
		 */
		void mouseMoveEvent(QMouseEvent* ev) override;

		/**
		 * @brief Double clicking the title bar also starts floating of the complete area.
		 */
		void mouseDoubleClickEvent(QMouseEvent* event) override;

		/**
		 * @brief Show context menu.
		 */
		void contextMenuEvent(QContextMenuEvent* event) override;

		/**
		 * @brief Handle resize events.
		 */
		void resizeEvent(QResizeEvent* event) override;

	public Q_SLOTS:
		/**
		 * @brief Call this slot to tell the title bar that it should update the tabs menu
		 * the next time it is shown.
		 */
		void markTabsMenuOutdated();

	public:
		/**
		 * @brief Alias for the base class.
		 */
		using Super = QFrame;

		/**
		 * @brief Returns the pointer to the tabBar().
		 */
		CDockAreaTabBar* tabBar() const;

		/**
		 * @brief Returns the button corresponding to the given title bar button identifier.
		 */
		CTitleBarButton* button(TitleBarButton which) const;

		/**
		 * @brief Returns the auto hide title label, used when the dock area is expanded and
		 * auto hidden.
		 */
		CElidingLabel* autoHideTitleLabel() const;

		/**
		 * @brief Returns the dock area widget that contains this title bar.
		 */
		CDockAreaWidget* dockAreaWidget() const;

		/**
		 * @brief Updates the visibility of the dock widget actions in the title bar.
		 */
		void updateDockWidgetActionsButtons();

		/**
		 * @brief Marks the tabs menu outdated before it calls its base class
		 * implementation.
		 */
		void setVisible(bool Visible) override;

		/**
		 * @brief Inserts a custom widget at position index into this title bar.
		 * If index is negative, the widget is added at the end.
		 * You can use this function to insert custom widgets into the title bar.
		 */
		void insertWidget(int index, QWidget* widget);

		/**
		 * @brief Searches for widget widget in this title bar.
		 * You can use this function, to get the position of the default
		 * widget in the tile bar.
		 * \code
		 * int tabBarIndex = TitleBar->indexOf(TitleBar->tabBar());
		 * int closeButtonIndex =
		 * TitleBar->indexOf(TitleBar->button(TitleBarButtonClose));
		 * \endcode
		 */
		int indexOf(QWidget* widget) const;

		/**
		 * @brief Close group tool tip based on the current state
		 * Auto hide widgets can only have one dock widget so it does not make sense
		 * for the tooltip to show close group.
		 */
		QString titleBarButtonToolTip(TitleBarButton Button) const;

		/**
		 * @brief Moves the dock area into its own floating widget if the area
		 * DockWidgetFloatable flag is true.
		 */
		void setAreaFloating();

		/**
		 * @brief Call this function, to create all the required auto hide controls.
		 */
		void showAutoHideControls(bool Show);

		/**
		 * @brief Returns true, if the auto hide controls are visible.
		 */
		bool isAutoHide() const;

		/**
		 * @brief Fills the provided menu with standard entries. If a nullptr is passed, a
		 * new menu is created and filled with standard entries.
		 * This function is called from the actual version of contextMenuEvent, but
		 * can be called from any code. Caller is responsible of deleting the created
		 * object.
		 *
		 * @param menu The QMenu to fill with standard entries. If nullptr, a new
		 * QMenu will be created.
		 * @return The filled QMenu, either the provided one or a newly created one if
		 * nullptr was passed.
		 */
		virtual QMenu* buildContextMenu(QMenu* menu);

	Q_SIGNALS:
		/**
		 * @brief This signal is emitted if a tab in the tab bar is clicked by the user
		 * or if the user clicks on a tab item in the title bar tab menu.
		 */
		void tabBarClicked(int index);

	private:
		friend struct DockAreaTitleBarPrivate;
		/// Private data (pimpl).
		DockAreaTitleBarPrivate* d;
};

}// namespace ads

// namespace ads
