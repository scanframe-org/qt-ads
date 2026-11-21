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

/// @file   CAutoHideSideBar_p.h
/// @author Syarif Fakhri
/// @date   05.09.2022
/// @brief  Private data of CAutoHideSideBar class.

#include "CAutoHideSideBar.h"

#include <QBoxLayout>
#include "CAutoHideTab.h"

namespace ads
{
class CTabsWidget;

/**
 * @brief Private data class of CSideTabBar class (pimpl).
 */
struct AutoHideSideBarPrivate
{
		/**
		 * @brief Private data constructor.
		 */
		AutoHideSideBarPrivate(CAutoHideSideBar* _public);

		/**
		 * @brief Convenience function to check if this is a horizontal sidebar.
		 */
		bool isHorizontal() const
		{
			return Qt::Horizontal == Orientation;
		}

		/**
		 * @brief Called from viewport to forward event handling to this.
		 */
		void handleViewportEvent(QEvent* e);

		/// @brief The this.
		CAutoHideSideBar* _this;
		/// @brief The container widget.
		CDockContainerWidget* ContainerWidget;
		/// @brief The tabs container widget.
		CTabsWidget* TabsContainerWidget;
		/// @brief The tabs' layout.
		QBoxLayout* TabsLayout;
		/// @brief The orientation.
		Qt::Orientation Orientation;
		/// @brief The side tab area.
		SideBarLocation SideTabArea = SideBarLocation::SideBarLeft;
};
}// namespace ads
