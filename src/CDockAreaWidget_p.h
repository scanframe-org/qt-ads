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

/// @file   CDockAreaWidget_p.h
/// @author Uwe Kindler
/// @date   24.02.2017
/// @brief  Private data of CDockAreaWidget class.

#include "CDockAreaWidget.h"

#include <QDebug>
#include <QList>
#include <QMenu>
#include <QPushButton>
#include <QScrollBar>
#include <QStackedLayout>
#include <QStyle>
#include <QXmlStreamWriter>

#include <CAutoHideDockContainer.h>
#include <CAutoHideTab.h>

#include "CDockAreaLayout.h"
#include "CDockAreaTabBar.h"
#include "CDockAreaTitleBar.h"
#include "CDockComponentsFactory.h"
#include "CDockContainerWidget.h"
#include "CDockManager.h"
#include "CDockOverlay.h"
#include "CDockSplitter.h"
#include "CDockWidget.h"
#include "CDockWidgetTab.h"
#include "CDockingStateReader.h"
#include "CElidingLabel.h"
#include "CFloatingDockContainer.h"

namespace ads
{
/**
 * @brief The index property.
 */
static const char* const INDEX_PROPERTY = "index";
/**
 * @brief The action property.
 */
static const char* const ACTION_PROPERTY = "action";

/**
 * @brief Check, if auto hide is enabled.
 */
static bool isAutoHideFeatureEnabled()
{
	return CDockManager::testAutoHideConfigFlag(CDockManager::AutoHideFeatureEnabled);
}

/**
 * @brief The default allowed areas.
 */
static const DockWidgetAreas DefaultAllowedAreas = AllDockAreas;

/**
 * @brief Private data class of CDockAreaWidget class (pimpl).
 */
struct DockAreaWidgetPrivate
{

		/**
		 * @brief Private data constructor.
		 */
		DockAreaWidgetPrivate(CDockAreaWidget* _public);

		/**
		 * @brief Convenience function to ease components factory access.
		 */
		QSharedPointer<ads::CDockComponentsFactory> componentsFactory() const
		{
			return DockManager->componentsFactory();
		}

		/**
		 * @brief Creates the layout for top area with tabs and close button.
		 */
		void createTitleBar();

		/**
		 * @brief Returns the dock widget with the given index.
		 */
		CDockWidget* dockWidgetAt(int index)
		{
			return qobject_cast<CDockWidget*>(ContentsLayout->widget(index));
		}

		/**
		 * @brief Convenience function to ease title widget access by index.
		 */
		CDockWidgetTab* tabWidgetAt(int index)
		{
			return dockWidgetAt(index)->tabWidget();
		}

		/**
		 * @brief Returns the tab action of the given dock widget.
		 */
		QAction* dockWidgetTabAction(CDockWidget* DockWidget) const
		{
			return qvariant_cast<QAction*>(DockWidget->property(ACTION_PROPERTY));
		}

		/**
		 * @brief Returns the index of the given dock widget.
		 */
		int dockWidgetIndex(CDockWidget* DockWidget) const
		{
			return DockWidget->property(INDEX_PROPERTY).toInt();
		}

		/**
		 * @brief Convenience function for tabbar access.
		 */
		CDockAreaTabBar* tabBar() const
		{
			return TitleBar->tabBar();
		}

		/**
		 * @brief Updates the enable state of the close and detach button.
		 */
		void updateTitleBarButtonStates();

		/**
		 * @brief Updates the enable state of the close and detach button.
		 */
		void updateTitleBarButtonVisibility(bool isTopLevel);

		/**
		 * @brief Scans all contained dock widgets for the max. minimum size hint.
		 */
		void updateMinimumSizeHint()
		{
			MinSizeHint = QSize();
			for (int i = 0; i < ContentsLayout->count(); ++i)
			{
				auto Widget = ContentsLayout->widget(i);
				MinSizeHint.setHeight(qMax(MinSizeHint.height(), Widget->minimumSizeHint().height()));
				MinSizeHint.setWidth(qMax(MinSizeHint.width(), Widget->minimumSizeHint().width()));
			}
		}

		/// @brief The this.
		CDockAreaWidget* _this = nullptr;
		/// @brief The layout.
		QBoxLayout* Layout = nullptr;
		/// @brief The contents layout.
		DockAreaLayout* ContentsLayout = nullptr;
		/// @brief The title bar.
		CDockAreaTitleBar* TitleBar = nullptr;
		/// @brief The dock manager.
		CDockManager* DockManager = nullptr;
		/// @brief The auto hide dock container.
		CAutoHideDockContainer* AutoHideDockContainer = nullptr;
		/// @brief The update title bar buttons.
		bool UpdateTitleBarButtons = false;
		/// @brief The allowed areas.
		DockWidgetAreas AllowedAreas = DefaultAllowedAreas;
		/// @brief The min size hint.
		QSize MinSizeHint;
		/// @brief The flags.
		CDockAreaWidget::DockAreaFlags Flags{CDockAreaWidget::DefaultFlags};
};
}// namespace ads
