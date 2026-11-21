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

/// @file   CAutoHideTab.h
/// @author Syarif Fakhri
/// @date   05.09.2022
/// @brief  Declaration of CAutoHideTab class.

#include "CPushButton.h"
#include "ads_globals.h"

namespace ads
{
struct AutoHideTabPrivate;
class CDockWidget;
class CAutoHideSideBar;
class CDockWidgetTab;
class DockContainerWidgetPrivate;

/**
 * @brief A dock widget Side tab that shows a title or an icon.
 * The dock widget tab is shown in the side tab bar to switch between
 * pinned dock widgets.
 */
class ADS_EXPORT CAutoHideTab : public CPushButton
{
		Q_OBJECT

		Q_PROPERTY(int sideBarLocation READ sideBarLocation)
		Q_PROPERTY(Qt::Orientation orientation READ orientation)
		Q_PROPERTY(bool activeTab READ isActiveTab)
		Q_PROPERTY(bool iconOnly READ iconOnly)

	public:
		/**
		 * @brief Default Constructor
		 * param[in] DockWidget The dock widget this title bar belongs to
		 * param[in] parent The parent widget of this title bar.
		 */
		CAutoHideTab(QWidget* parent = nullptr);

		/**
		 * @brief Virtual Destructor.
		 */
		~CAutoHideTab() override;

	private Q_SLOTS:
		/**
		 * @brief Slot invoked on auto hide to action clicked.
		 */
		void onAutoHideToActionClicked();

		/**
		 * @brief Slot handling: drag hover delay expired.
		 */
		void onDragHoverDelayExpired();

	protected:
		/**
		 * @brief Sets the sidebar.
		 */
		void setSideBar(CAutoHideSideBar* SideTabBar);

		/**
		 * @brief Removes the tab from its sidebar.
		 */
		void removeFromSideBar();

		/**
		 * @brief Handles generic events.
		 */
		bool event(QEvent* event) override;

		/**
		 * @brief Handles the context menu event.
		 */
		void contextMenuEvent(QContextMenuEvent* ev) override;

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
		 * @brief Handles the drag enter event.
		 */
		void dragEnterEvent(QDragEnterEvent* ev) override;
		/**
		 * @brief Handles the drag leave event.
		 */
		void dragLeaveEvent(QDragLeaveEvent* ev) override;

	public:
		/**
		 * @brief Alias for the base class.
		 */
		using Super = CPushButton;

		/**
		 * @brief Update stylesheet style if a property changes.
		 */
		void updateStyle();

		/**
		 * @brief Getter for side tab bar area property.
		 */
		SideBarLocation sideBarLocation() const;

		/**
		 * @brief Set orientation vertical or horizontal.
		 */
		void setOrientation(Qt::Orientation Orientation);

		/**
		 * @brief Returns the current orientation.
		 */
		Qt::Orientation orientation() const;

		/**
		 * @brief Returns true, if this is the active tab. The tab is active if the auto
		 * hide widget is visible.
		 */
		bool isActiveTab() const;

		/**
		 * @brief Returns the dock widget this belongs to.
		 */
		CDockWidget* dockWidget() const;

		/**
		 * @brief Sets the dock widget that is controlled by this tab.
		 */
		void setDockWidget(CDockWidget* DockWidget);

		/**
		 * @brief Returns true if the auto hide config flag AutoHideSideBarsIconOnly
		 * is set and if the tab has an icon - that means the icon is not null.
		 */
		bool iconOnly() const;

		/**
		 * @brief Returns the sidebar that contains this tab or a nullptr if the tab is
		 * not in a sidebar.
		 */
		CAutoHideSideBar* sideBar() const;

		/**
		 * @brief Returns the index of this tab in the sideBar.
		 */
		int tabIndex() const;

	public Q_SLOTS:
		/**
		 * @brief Set the dock widget floating, if it is floatable.
		 */
		void setDockWidgetFloating();

		/**
		 * @brief Unpin and dock the auto hide widget.
		 */
		void unpinDockWidget();

		/**
		 * @brief Calls the requestCloseDockWidget() function for the assigned dock widget.
		 */
		void requestCloseDockWidget();

	private:
		friend struct AutoHideTabPrivate;
		friend class CDockWidget;
		friend class CAutoHideDockContainer;
		friend class CAutoHideSideBar;
		friend class CDockAreaWidget;
		friend class CDockContainerWidget;
		/// @brief The dock container widget private.
		friend DockContainerWidgetPrivate;
		/// Private data (pimpl).
		AutoHideTabPrivate* d;
};
}// namespace ads

// namespace ads
