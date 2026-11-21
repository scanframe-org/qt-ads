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

/// @file   CDockAreaTabBar.h
/// @author Uwe Kindler
/// @date   24.08.2018
/// @brief  Declaration of CDockAreaTabBar class.

#include <QScrollArea>

#include "ads_globals.h"

namespace ads
{
class CDockAreaWidget;
class CDockWidgetTab;
struct DockAreaTabBarPrivate;
class CDockAreaTitleBar;
class CFloatingDockContainer;
class IFloatingWidget;

/**
 * @brief Custom tabbar implementation for tab area that is shown on top of a
 * dock area widget.
 * The tabbar displays the tab widgets of the contained dock widgets.
 * We cannot use QTabBar here because it does a lot of fancy animations
 * that will crash the application if a tab is removed while the animation
 * has not finished. And we need to remove a tab, if the user drags a
 * a dock widget out of a group of tabbed widgets.
 */
class ADS_EXPORT CDockAreaTabBar : public QScrollArea
{
		Q_OBJECT

	public:
		/**
		 * @brief Default Constructor.
		 */
		CDockAreaTabBar(CDockAreaWidget* parent);

		/**
		 * @brief Virtual Destructor.
		 */
		~CDockAreaTabBar() override;

	private Q_SLOTS:
		/**
		 * @brief Slot invoked on tab clicked.
		 */
		void onTabClicked();
		/**
		 * @brief Slot invoked on tab close requested.
		 */
		void onTabCloseRequested();
		/**
		 * @brief Slot invoked on close other tabs requested.
		 */
		void onCloseOtherTabsRequested();
		/**
		 * @brief Slot handling: tab widget moved.
		 */
		void onTabWidgetMoved(const QPoint& GlobalPos);

	protected:
		/**
		 * @brief Handles the wheel event.
		 */
		void wheelEvent(QWheelEvent* Event) override;

	public:
		/**
		 * @brief Alias for the base class.
		 */
		using Super = QScrollArea;

		/**
		 * @brief Inserts the given dock widget tab at the given position.
		 * Inserting a new tab at an index less than or equal to the current index
		 * will increment the current index, but keep the current tab.
		 */
		void insertTab(int Index, CDockWidgetTab* Tab);

		/**
		 * @brief Removes the given DockWidgetTab from the tabbar.
		 */
		void removeTab(CDockWidgetTab* Tab);

		/**
		 * @brief Returns the number of tabs in this tabbar.
		 */
		int count() const;

		/**
		 * @brief Returns the current index or -1 if no tab is selected.
		 */
		int currentIndex() const;

		/**
		 * @brief Returns the current tab or a nullptr if no tab is selected.
		 */
		CDockWidgetTab* currentTab() const;

		/**
		 * @brief Returns the tab with the given index.
		 */
		CDockWidgetTab* tab(int Index) const;

		/**
		 * @brief Returns the tab at the given position.
		 * Returns -1 if the position is left of the first tab and count() if the
		 * position is right of the last tab. Returns -2 to indicate an invalid
		 * value.
		 */
		int tabAt(const QPoint& Pos) const;

		/**
		 * @brief Returns the tab insertion index for the given mouse cursor position.
		 */
		int tabInsertIndexAt(const QPoint& Pos) const;

		/**
		 * @brief Filters the tab widget events.
		 */
		bool eventFilter(QObject* watched, QEvent* event) override;

		/**
		 * @brief This function returns true if the tab is open, that means if it is
		 * visible to the user. If the function returns false, the tab is
		 * closed.
		 */
		bool isTabOpen(int Index) const;

		/**
		 * @brief Overrides the minimumSizeHint() function of QScrollArea
		 * The minimumSizeHint() is bigger than the sizeHint () for the scroll
		 * area because even if the scrollbars are invisible, the required space
		 * is reserved in the minimumSizeHint(). This override simply returns
		 * sizeHint();.
		 */
		QSize minimumSizeHint() const override;

		/**
		 * @brief The function provides a sizeHint that matches the height of the
		 * internal viewport.
		 */
		QSize sizeHint() const override;

		/**
		 * @brief This function returns true, if the tabs need more space than the size
		 * of the tab bar.
		 */
		bool areTabsOverflowing() const;

	public Q_SLOTS:
		/**
		 * @brief This property sets the index of the tab bar's visible tab.
		 */
		void setCurrentIndex(int Index);

		/**
		 * @brief This function will close the tab given in Index param.
		 * Closing a tab means, the tab will be hidden, it will not be removed.
		 */
		void closeTab(int Index);

	Q_SIGNALS:
		/**
		 * @brief This signal is emitted when the tab bar's current tab is about to be
		 * changed. The new current has the given index, or -1 if there isn't a new
		 * one.
		 */
		void currentChanging(int Index);

		/**
		 * @brief This signal is emitted when the tab bar's current tab changes. The new
		 * current has the given index, or -1 if there isn't a new one.
		 */
		void currentChanged(int Index);

		/**
		 * @brief This signal is emitted when user clicks on a tab.
		 */
		void tabBarClicked(int index);

		/**
		 * @brief This signal is emitted when the close button on a tab is clicked.
		 * The index is the index that should be closed.
		 */
		void tabCloseRequested(int index);

		/**
		 * @brief This signal is emitted if a tab has been closed.
		 */
		void tabClosed(int index);

		/**
		 * @brief This signal is emitted if a tab has been opened.
		 * A tab is opened if it has been made visible.
		 */
		void tabOpened(int index);

		/**
		 * @brief This signal is emitted when the tab has moved the tab at index position
		 * from to index position to.
		 */
		void tabMoved(int from, int to);

		/**
		 * @brief This signal is emitted, just before the tab with the given index is
		 * removed.
		 */
		void removingTab(int index);

		/**
		 * @brief This signal is emitted if a tab has been inserted.
		 */
		void tabInserted(int index);

		/**
		 * @brief This signal is emitted when a tab title elide state has been changed.
		 */
		void elidedChanged(bool elided);

	private:
		friend struct DockAreaTabBarPrivate;
		friend class CDockAreaTitleBar;
		/// Private data (pimpl).
		DockAreaTabBarPrivate* d;
};
}// namespace ads
