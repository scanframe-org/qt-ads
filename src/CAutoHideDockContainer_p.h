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

/// @file   CAutoHideDockContainer_p.h
/// @author Syarif Fakhri
/// @date   05.09.2022
/// @brief  Private data of CAutoHideDockContainer class.

#include "CAutoHideDockContainer.h"

#include <QBoxLayout>
#include <QPointer>
#include <QSplitter>
#include "CAutoHideTab.h"
#include "CDockAreaWidget.h"
#include "CDockComponentsFactory.h"
#include "CDockManager.h"
#include "CResizeHandle.h"

namespace ads
{
/**
 * @brief The resize margin.
 */
static const int ResizeMargin = 30;

/**
 * @brief Implements is horizontal area.
 */
bool static isHorizontalArea(SideBarLocation Area)
{
	switch (Area)
	{
		case SideBarLocation::SideBarTop:
		case SideBarLocation::SideBarBottom:
			return true;
		case SideBarLocation::SideBarLeft:
		case SideBarLocation::SideBarRight:
			return false;
	}
	return true;
}

/**
 * @brief Implements edge from side tab bar area.
 */
Qt::Edge static edgeFromSideTabBarArea(SideBarLocation Area)
{
	switch (Area)
	{
		case SideBarLocation::SideBarTop:
			return Qt::BottomEdge;
		case SideBarLocation::SideBarBottom:
			return Qt::TopEdge;
		case SideBarLocation::SideBarLeft:
			return Qt::RightEdge;
		case SideBarLocation::SideBarRight:
			return Qt::LeftEdge;
		case SideBarLocation::SideBarNone:
			break;
	}
	return Qt::LeftEdge;
}

/**
 * @brief Implements resize handle layout position.
 */
int resizeHandleLayoutPosition(SideBarLocation Area)
{
	switch (Area)
	{
		case SideBarLocation::SideBarBottom:
		case SideBarLocation::SideBarRight:
			return 0;

		case SideBarLocation::SideBarTop:
		case SideBarLocation::SideBarLeft:
			return 1;

		default:
			return 0;
	}

	return 0;
}

/**
 * @brief Private data of CAutoHideDockContainer - pimpl.
 */
struct AutoHideDockContainerPrivate
{

		/**
		 * @brief Private data constructor.
		 */
		AutoHideDockContainerPrivate(CAutoHideDockContainer* _public);

		/**
		 * @brief Convenience function to ease access to dock manager components factory.
		 */
		QSharedPointer<ads::CDockComponentsFactory> componentsFactory() const
		{
			if (!DockWidget || !DockWidget->dockManager())
			{
				return CDockComponentsFactory::factory();
			}
			else
			{
				return DockWidget->dockManager()->componentsFactory();
			}
		}

		/**
		 * @brief Convenience function to get a dock widget area.
		 */
		DockWidgetArea getDockWidgetArea(SideBarLocation area)
		{
			switch (area)
			{
				case SideBarLocation::SideBarLeft:
					return LeftDockWidgetArea;
				case SideBarLocation::SideBarRight:
					return RightDockWidgetArea;
				case SideBarLocation::SideBarBottom:
					return BottomDockWidgetArea;
				case SideBarLocation::SideBarTop:
					return TopDockWidgetArea;
				default:
					return LeftDockWidgetArea;
			}

			return LeftDockWidgetArea;
		}

		/**
		 * @brief Update the resize limit of the resize handle.
		 */
		void updateResizeHandleSizeLimitMax()
		{
			auto Rect = _this->dockContainer()->contentRect();
			const auto maxResizeHandleSize = ResizeHandle->orientation() == Qt::Horizontal ? Rect.width() : Rect.height();
			ResizeHandle->setMaxResizeSize(maxResizeHandleSize - ResizeMargin);
		}

		/**
		 * @brief Convenience function to check, if this is an horizontal area.
		 */
		bool isHorizontal() const
		{
			return isHorizontalArea(SideTabBarArea);
		}

		/**
		 * @brief Forward this event to the dock container.
		 */
		void forwardEventToDockContainer(QEvent* event)
		{
			if (auto DockContainer = _this->dockContainer())
			{
				DockContainer->handleAutoHideWidgetEvent(event, _this);
			}
		}

		/// @brief The this.
		CAutoHideDockContainer* _this;
		/// @brief The dock area.
		CDockAreaWidget* DockArea{nullptr};
		/// @brief The dock widget.
		CDockWidget* DockWidget{nullptr};
		/// @brief The side tab bar area.
		SideBarLocation SideTabBarArea = SideBarNone;
		/// @brief The layout.
		QBoxLayout* Layout = nullptr;
		/// @brief The resize handle.
		CResizeHandle* ResizeHandle = nullptr;
		// creates invalid size
		/// @brief The size.
		QSize Size;
		/// @brief The side tab.
		QPointer<CAutoHideTab> SideTab;
		/// @brief The size cache.
		QSize SizeCache;
};
}// namespace ads
