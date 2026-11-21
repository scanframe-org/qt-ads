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

/// @file   CAutoHideDockContainer.h
/// @author Syarif Fakhri
/// @date   05.09.2022
/// @brief  Declaration of CAutoHideDockContainer class.

#include <QSplitter>

#include "CAutoHideTab.h"
#include "ads_globals.h"

QT_FORWARD_DECLARE_CLASS(QXmlStreamWriter)

namespace ads
{
struct AutoHideDockContainerPrivate;
class CDockManager;
class CDockWidget;
class CDockContainerWidget;
class CAutoHideSideBar;
class CDockAreaWidget;
class CDockingStateReader;
struct SideTabBarPrivate;

/**
 * @brief Auto hide container for hosting an auto hide dock widget.
 */
class ADS_EXPORT CAutoHideDockContainer : public QFrame
{
		Q_OBJECT
		Q_PROPERTY(int sideBarLocation READ sideBarLocation)

	public:
		/**
		 * @brief Create Auto Hide widget with the given dock widget.
		 */
		CAutoHideDockContainer(CDockWidget* DockWidget, SideBarLocation area, CDockContainerWidget* parent);

		/**
		 * @brief Virtual Destructor.
		 */
		~CAutoHideDockContainer() override;
		/**
		 * @brief Filters events of watched objects.
		 */
		bool eventFilter(QObject* watched, QEvent* event) override;

	protected:
		/**
		 * @brief Handles the resize event.
		 */
		void resizeEvent(QResizeEvent* event) override;
		/**
		 * @brief Handles the leave event.
		 */
		void leaveEvent(QEvent* event) override;
		/**
		 * @brief Handles generic events.
		 */
		bool event(QEvent* event) override;
		/**
		 * @brief Handles the drag leave event.
		 */
		void dragLeaveEvent(QDragLeaveEvent* ev) override;

		/**
		 * @brief Updates the size considering the size limits and the resize margins.
		 */
		void updateSize();

		/*
		 * Saves the state and size
		 */
		/**
		 * @brief Implements save state.
		 */
		void saveState(QXmlStreamWriter& Stream);

	public:
		/**
		 * @brief Alias for the base class.
		 */
		using Super = QFrame;

		/**
		 * @brief Gets the side tab bar.
		 */
		CAutoHideSideBar* autoHideSideBar() const;

		/**
		 * @brief Returns the side tab.
		 */
		CAutoHideTab* autoHideTab() const;

		/**
		 * @brief Gets the dock widget in this dock container.
		 */
		CDockWidget* dockWidget() const;

		/**
		 * @brief Returns the index of this container in the sidebar.
		 */
		int tabIndex() const;

		/**
		 * @brief Adds a dock widget and removes the previous dock widget.
		 */
		void addDockWidget(CDockWidget* DockWidget);

		/**
		 * @brief Returns the side tab bar area of this Auto Hide dock container.
		 */
		SideBarLocation sideBarLocation() const;

		/**
		 * @brief Sets a new SideBarLocation.
		 * If a new sidebar location is set, the auto hide dock container needs
		 * to update its resize handle position.
		 */
		void setSideBarLocation(SideBarLocation SideBarLocation);

		/**
		 * @brief Returns the dock area widget of this Auto Hide dock container.
		 */
		CDockAreaWidget* dockAreaWidget() const;

		/**
		 * @brief Returns the parent container that hosts this auto hide container.
		 */
		CDockContainerWidget* dockContainer() const;

		/**
		 * @brief Moves the contents to the parent container widget
		 * Used before removing this Auto Hide dock container.
		 */
		void moveContentsToParent();

		/**
		 * @brief Cleanups up the side tab widget and then deletes itself.
		 */
		void cleanupAndDelete();

		/*
		 * Toggles the auto Hide dock container widget
		 * This will also hide the side tab widget
		 */
		/**
		 * @brief Implements toggle view.
		 */
		void toggleView(bool Enable);

		/*
		 * Collapses the auto hide dock container widget
		 * Does not hide the side tab widget
		 */
		/**
		 * @brief Implements collapse view.
		 */
		void collapseView(bool Enable);

		/**
		 * @brief Toggles the current collapse state.
		 */
		void toggleCollapseState();

		/**
		 * @brief Use this instead of resize.
		 * Depending on the sidebar location this will set the width or height
		 * of this auto hide container.
		 */
		void setSize(int Size);

		/**
		 * @brief Resets the width or height to the initial dock widget size depending on
		 * the orientation.
		 * If the orientation is Qt::Horizontal, then the height is reset to
		 * the initial size and if orientation is Qt::Vertical, then the width is
		 * reset to the initial size.
		 */
		void resetToInitialDockWidgetSize();

		/**
		 * @brief Returns orientation of this container.
		 * Left and right containers have a Qt::Vertical orientation and top / bottom
		 * containers have a Qt::Horizontal orientation.
		 * The function returns the orientation of the corresponding auto hide sidebar.
		 */
		Qt::Orientation orientation() const;

		/**
		 * @brief Removes the AutoHide container from the current sidebar and adds
		 * it to the new sidebar given in SideBarLocation.
		 */
		void moveToNewSideBarLocation(SideBarLocation SideBarLocation, int TabIndex = -1);

	private:
		friend struct AutoHideDockContainerPrivate;
		/// @brief The c auto hide sidebar.
		friend CAutoHideSideBar;
		/// @brief The side tab bar private.
		friend SideTabBarPrivate;
		/// Private data (pimpl).
		AutoHideDockContainerPrivate* d;
};
}// namespace ads
