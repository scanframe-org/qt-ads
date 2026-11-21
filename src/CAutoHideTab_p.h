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

#pragma once

/// @file   CAutoHideTab_p.h
/// @author Syarif Fakhri
/// @date   05.09.2022
/// @brief  Private data of CAutoHideTab class.

#include "CAutoHideTab.h"

#include <QApplication>
#include <QBoxLayout>
#include <QElapsedTimer>
#include <QEvent>
#include <QMenu>
#include <QTimer>

#include "CAutoHideDockContainer.h"
#include "CAutoHideSideBar.h"
#include "CDockAreaWidget.h"
#include "CDockManager.h"
#include "CDockOverlay.h"
#include "CDockWidget.h"
#include "CFloatingDragPreview.h"
#include "ads_globals.h"

namespace ads
{

/**
 * @brief Private data class of CDockWidgetTab class (pimpl).
 */
struct AutoHideTabPrivate
{

		/**
		 * @brief Private data constructor.
		 */
		AutoHideTabPrivate(CAutoHideTab* _public);

		/**
		 * @brief Update the orientation, visibility and spacing based on the area of
		 * the side bar.
		 */
		void updateOrientation();

		/**
		 * @brief Convenience function to ease dock container access.
		 */
		CDockContainerWidget* dockContainer() const
		{
			return DockWidget ? DockWidget->dockContainer() : nullptr;
		}

		/**
		 * @brief Forward this event to the dock container.
		 */
		void forwardEventToDockContainer(QEvent* event)
		{
			auto DockContainer = dockContainer();
			if (DockContainer)
			{
				DockContainer->handleAutoHideWidgetEvent(event, _this);
			}
		}

		/**
		 * @brief Helper function to create and initialize the menu entries for
		 * the "Auto Hide Group To..." menu.
		 */
		QAction* createAutoHideToAction(const QString& Title, SideBarLocation Location, QMenu* Menu)
		{
			auto Action = Menu->addAction(Title);
			Action->setProperty(internal::LocationProperty, Location);
			QObject::connect(Action, &QAction::triggered, _this, &CAutoHideTab::onAutoHideToActionClicked);
			Action->setEnabled(Location != _this->sideBarLocation());
			return Action;
		}

		/**
		 * @brief Test function for current drag state.
		 */
		bool isDraggingState(eDragState dragState) const
		{
			return this->DragState == dragState;
		}

		/**
		 * @brief Saves the drag start position in global and local coordinates.
		 */
		void saveDragStartMousePosition(const QPoint& GlobalPos)
		{
			GlobalDragStartMousePosition = GlobalPos;
			DragStartMousePosition = _this->mapFromGlobal(GlobalPos);
		}

		/**
		 * @brief Starts floating of the dock widget that belongs to this title bar
		 * Returns true, if floating has been started and false if floating
		 * is not possible for any reason.
		 */
		bool startFloating(eDragState DraggingState = DraggingFloatingWidget);

		template<typename T>
		/**
		 * @brief Creates floating widget.
		 */
		IFloatingWidget* createFloatingWidget(T* Widget)
		{
			auto w = new CFloatingDragPreview(Widget);
			_this->connect(w, &CFloatingDragPreview::draggingCanceled, [this]() {
				DragState = DraggingInactive;
			});
			return w;
		}

		/// @brief The this.
		CAutoHideTab* _this;
		/// @brief The dock widget.
		CDockWidget* DockWidget = nullptr;
		/// @brief The side bar.
		CAutoHideSideBar* SideBar = nullptr;
		/// @brief The orientation.
		Qt::Orientation Orientation{Qt::Vertical};
		/// @brief The time since hover mouse press.
		QElapsedTimer TimeSinceHoverMousePress;
		/// @brief The drag over timer.
		QTimer DragOverTimer;
		/// @brief The mouse pressed.
		bool MousePressed = false;
		/// @brief The drag state.
		eDragState DragState = DraggingInactive;
		/// @brief The global drag start mouse position.
		QPoint GlobalDragStartMousePosition;
		/// @brief The drag start mouse position.
		QPoint DragStartMousePosition;
		/// @brief The floating widget.
		IFloatingWidget* FloatingWidget = nullptr;
		/// @brief The drag start orientation.
		Qt::Orientation DragStartOrientation;
};
}// namespace ads
