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

/// @file   CDockAreaTabBar_p.h
/// @author Uwe Kindler
/// @date   24.08.2018
/// @brief  Private data of CDockAreaTabBar class.

#include "CDockAreaTabBar.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QMouseEvent>
#include <QPointer>
#include <QScrollBar>
#include <QTimer>
#include <QtGlobal>

#include <iostream>

#include "CDockAreaWidget.h"
#include "CDockManager.h"
#include "CDockOverlay.h"
#include "CDockWidget.h"
#include "CDockWidgetTab.h"
#include "CFloatingDockContainer.h"
#include "CFloatingDragPreview.h"

namespace ads
{
/**
 * @brief Private data class of CDockAreaTabBar class (pimpl).
 */
struct DockAreaTabBarPrivate
{

		/**
		 * @brief Private data constructor.
		 */
		DockAreaTabBarPrivate(CDockAreaTabBar* _public);

		/**
		 * @brief Update tabs after current index changed or when tabs are removed.
		 * The function reassigns the stylesheet to update the tabs.
		 */
		void updateTabs();

		/**
		 * @brief Convenience function to access first tab.
		 */
		CDockWidgetTab* firstTab() const
		{
			return _this->tab(0);
		}

		/**
		 * @brief Convenience function to access last tab.
		 */
		CDockWidgetTab* lastTab() const
		{
			return _this->tab(_this->count() - 1);
		}

		/// @brief The this.
		CDockAreaTabBar* _this;
		/// @brief The dock area.
		CDockAreaWidget* DockArea;
		/// @brief The tabs container widget.
		QWidget* TabsContainerWidget;
		/// @brief The tabs layout.
		QBoxLayout* TabsLayout;
		/// @brief The current index.
		int CurrentIndex = -1;
};
}// namespace ads
