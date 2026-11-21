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

/// @file   CDockWidget_p.h
/// @author Uwe Kindler
/// @date   26.02.2017
/// @brief  Private data of CDockWidget class.

#include "CDockWidget.h"

#include <QAction>
#include <QBoxLayout>
#include <QDebug>
#include <QPointer>
#include <QScrollArea>
#include <QToolBar>
#include <CAutoHideTab.h>
#include "CDockAreaWidget.h"
#include "CDockComponentsFactory.h"
#include "CDockManager.h"
#include "CDockWidgetTab.h"
#include "ads_globals.h"

namespace ads
{
/**
 * @brief Private data class of CDockWidget class (pimpl).
 */
struct DockWidgetPrivate
{
		/**
		 * @brief Declaration of the WidgetFactory class.
		 */
		struct WidgetFactory
		{
				/// @brief Create widget.
				CDockWidget::FactoryFunc createWidget;
				/// @brief The insert mode.
				CDockWidget::eInsertMode insertMode;
		};

		/**
		 * @brief Private data constructor.
		 */
		DockWidgetPrivate(CDockWidget* _public);

		/**
		 * @brief Convenience function to ease components factory access.
		 */
		QSharedPointer<CDockComponentsFactory> componentsFactory() const;

		/**
		 * @brief Show dock widget.
		 */
		void showDockWidget();

		/**
		 * @brief Hide dock widget.
		 */
		void hideDockWidget();

		/**
		 * @brief Hides a dock area if all dock widgets in the area are closed.
		 * This function updates the current selected tab and hides the parent
		 * dock area if it is empty.
		 */
		void updateParentDockArea();

		/**
		 * @brief Closes all auto hide dock widgets if there are no more opened dock areas
		 * This prevents the auto hide dock widgets from being pinned to an empty dock
		 * area.
		 */
		void closeAutoHideDockWidgetsIfNeeded();

		/**
		 * @brief Set up the top toolbar.
		 */
		void setupToolBar();

		/**
		 * @brief Set up the main scroll area.
		 */
		void setupScrollArea();

		/**
		 * @brief Creates the content widget with the registered widget factory and
		 * returns true on success.
		 */
		bool createWidgetFromFactory();

		/**
		 * @brief Use the dock manager toolbar style and icon size for the different states.
		 */
		void setToolBarStyleFromDockManager();

		/// @brief The this.
		CDockWidget* _this = nullptr;
		/// @brief The layout.
		QBoxLayout* Layout = nullptr;
		/// @brief The widget.
		QWidget* Widget = nullptr;
		/// @brief The tab widget.
		CDockWidgetTab* TabWidget = nullptr;
		/// @brief The features.
		CDockWidget::DockWidgetFeatures Features = CDockWidget::DefaultDockWidgetFeatures;
		/// @brief The dock manager.
		QPointer<CDockManager> DockManager;
		/// @brief The dock area.
		QPointer<CDockAreaWidget> DockArea;
		/// @brief The toggle view action.
		QAction* ToggleViewAction = nullptr;
		/// @brief The closed.
		bool Closed = false;
		/// @brief The scroll area.
		QScrollArea* ScrollArea = nullptr;
		/// @brief The toolbar.
		QToolBar* ToolBar = nullptr;
		/// @brief The toolbar style docked.
		Qt::ToolButtonStyle ToolBarStyleDocked = Qt::ToolButtonIconOnly;
		/// @brief The toolbar style floating.
		Qt::ToolButtonStyle ToolBarStyleFloating = Qt::ToolButtonTextUnderIcon;
		/// @brief The toolbar icon size docked.
		QSize ToolBarIconSizeDocked = QSize(16, 16);
		/// @brief The toolbar icon size floating.
		QSize ToolBarIconSizeFloating = QSize(24, 24);
		/// @brief The is floating top level.
		bool IsFloatingTopLevel = false;
		/// @brief The title bar actions.
		QList<QAction*> TitleBarActions;
		/// @brief The minimum size hint mode.
		CDockWidget::eMinimumSizeHintMode MinimumSizeHintMode = CDockWidget::MinimumSizeHintFromDockWidget;
		/// @brief The factory.
		WidgetFactory* Factory = nullptr;
		/// @brief The side tab widget.
		QPointer<CAutoHideTab> SideTabWidget;
		/// @brief The toolbar style source.
		CDockWidget::eToolBarStyleSource ToolBarStyleSource = CDockWidget::ToolBarStyleFromDockManager;
		/// @brief The preferred auto hide sidebar location.
		SideBarLocation PreferredAutoHideSideBarLocation = SideBarNone;
};
}// namespace ads
