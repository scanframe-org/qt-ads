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

#include "CDockOverlay.h"

#include <QCursor>
#include <QDebug>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QMap>
#include <QMoveEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPointer>
#include <QResizeEvent>
#include <QWindow>
#include <QtGlobal>

#include <iostream>

#include "CAutoHideSideBar.h"
#include "CDockAreaTabBar.h"
#include "CDockAreaTitleBar.h"
#include "CDockAreaWidget.h"
#include "CDockContainerWidget.h"
#include "CDockManager.h"

namespace ads
{
static const int AutoHideAreaWidth = 32;
static const int AutoHideAreaMouseZone = 8;
static const int InvalidTabIndex = -2;

/**
 * @brief Private data class of CDockOverlay.
 */
struct DockOverlayPrivate
{

		/**
		 * @brief Private data constructor.
		 */
		DockOverlayPrivate(CDockOverlay* _public);

		/**
		 * @brief Returns the overlay width / height depending on the visibility
		 * of the sidebar.
		 */
		int sideBarOverlaySize(SideBarLocation sideBarLocation);

		/**
		 * @brief The area where the mouse is considered in the sidebar.
		 */
		int sideBarMouseZone(SideBarLocation sideBarLocation);
		/// @brief The this.
		CDockOverlay* _this;
		/// @brief The allowed areas.
		DockWidgetAreas AllowedAreas = InvalidDockWidgetArea;
		/// @brief The cross.
		CDockOverlayCross* Cross;
		/// @brief The target widget.
		QPointer<QWidget> TargetWidget;
		/// @brief The last location.
		DockWidgetArea LastLocation = InvalidDockWidgetArea;
		/// @brief The drop preview enabled.
		bool DropPreviewEnabled = true;
		/// @brief The mode.
		CDockOverlay::eMode Mode = CDockOverlay::ModeDockAreaOverlay;
		/// @brief The drop area rect.
		QRect DropAreaRect;
		/// @brief The tab index.
		int TabIndex = InvalidTabIndex;
		// Wayland: the global cursor position is not available, so the last drop
		// position from the drag and drop events is cached here to drive the
		// drop preview rectangle and the highlighted drop indicator
		/// @brief The last global pos.
		QPoint LastGlobalPos;
		// Wayland: the overlay is temporarily reparented into whatever top level
		// window it is shown over (see showOverlay), because a child widget cannot
		// render over a foreign window. This is the stable home window (the dock
		// manager's window) that the overlay is reparented back to when hidden, so
		// it is never left as a child of a transient floating window that gets
		// destroyed - which would delete this dock-manager-owned overlay with it.
		/// @brief The home window.
		QPointer<QWidget> HomeWindow;
};
}// namespace ads
