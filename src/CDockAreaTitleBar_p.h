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

/// @file   CDockAreaTitleBar_p.h
/// @author Uwe Kindler
/// @date   12.10.2018
/// @brief  Declaration of classes CTitleBarButton and CSpacerWidget.

#include <QFrame>

#include "ads_globals.h"
#include "CDockAreaTitleBar.h"
#include "CSpacerWidget.h"
#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QMenu>
#include <QMouseEvent>
#include <QPointer>
#include <QPushButton>
#include <QScrollArea>
#include <QStyle>
#include <QToolButton>
#include <iostream>
#include "CAutoHideDockContainer.h"
#include "CDockAreaTabBar.h"
#include "CDockAreaWidget.h"
#include "CDockComponentsFactory.h"
#include "CDockFocusController.h"
#include "CDockManager.h"
#include "CDockOverlay.h"
#include "CDockWidget.h"
#include "CDockWidgetTab.h"
#include "CElidingLabel.h"
#include "CFloatingDockContainer.h"
#include "CFloatingDragPreview.h"
#include "CIconProvider.h"

namespace ads
{

/**
 * @brief Private data class of CDockAreaTitleBar class (pimpl).
 */
struct DockAreaTitleBarPrivate
{

		/**
		 * @brief Private data constructor.
		 */
		DockAreaTitleBarPrivate(CDockAreaTitleBar* _public);

		/**
		 * @brief Creates the title bar close and menu buttons.
		 */
		void createButtons();

		/**
		 * @brief Creates the auto hide title label, only displayed when the dock area is
		 * overlayed.
		 */
		void createAutoHideTitleLabel();

		/**
		 * @brief Creates the internal TabBar.
		 */
		void createTabBar();

		/**
		 * @brief Convenience function for DockManager access.
		 */
		CDockManager* dockManager() const
		{
			return DockArea->dockManager();
		}

		/**
		 * @brief Convenience function for access to dock manager components factory.
		 */
		QSharedPointer<ads::CDockComponentsFactory> componentsFactory() const
		{
			return dockManager()->componentsFactory();
		}

		/**
		 * @brief Returns true if the given config flag is set
		 * Convenience function to ease config flag testing.
		 */
		static bool testConfigFlag(CDockManager::eConfigFlag Flag)
		{
			return CDockManager::testConfigFlag(Flag);
		}

		/**
		 * @brief Returns true if the given config flag is set
		 * Convenience function to ease config flag testing.
		 */
		static bool testAutoHideConfigFlag(CDockManager::eAutoHideFlag Flag)
		{
			return CDockManager::testAutoHideConfigFlag(Flag);
		}

		/**
		 * @brief Test function for current drag state.
		 */
		bool isDraggingState(eDragState dragState) const
		{
			return this->DragState == dragState;
		}

		/**
		 * @brief Starts floating.
		 */
		void startFloating(const QPoint& Offset);

		/**
		 * @brief Makes the dock area floating.
		 */
		IFloatingWidget* makeAreaFloating(const QPoint& Offset, eDragState DragState);

		/**
		 * @brief Wayland hybrid drag: drive the in-window drag preview from reliable event
		 * coordinates and, when the cursor leaves the source top-level window,
		 * convert the in-window drag into a native compositor platform drag
		 * (mirrors CDockWidgetTab).
		 */
		void waylandPreviewMove(QMouseEvent* ev);

		/**
		 * @brief Helper function to create and initialize the menu entries for
		 * the "Auto Hide Group To..." menu.
		 */
		QAction* createAutoHideToAction(const QString& Title, SideBarLocation Location, QMenu* Menu)
		{
			auto Action = Menu->addAction(Title);
			Action->setProperty(internal::LocationProperty, Location);
			QObject::connect(Action, &QAction::triggered, _this, &CDockAreaTitleBar::onAutoHideToActionClicked);
			return Action;
		}

		/// @brief The this.
		CDockAreaTitleBar* _this;
		/// @brief The tabs menu button.
		QPointer<CTitleBarButton> TabsMenuButton;
		/// @brief The auto hide button.
		QPointer<CTitleBarButton> AutoHideButton;
		/// @brief The undock button.
		QPointer<CTitleBarButton> UndockButton;
		/// @brief The close button.
		QPointer<CTitleBarButton> CloseButton;
		/// @brief The minimize button.
		QPointer<CTitleBarButton> MinimizeButton;
		/// @brief The layout.
		QBoxLayout* Layout;
		/// @brief The dock area.
		CDockAreaWidget* DockArea;
		/// @brief The tab bar.
		CDockAreaTabBar* TabBar;
		/// @brief The auto hide title label.
		CElidingLabel* AutoHideTitleLabel = nullptr;
		/// @brief The menu outdated.
		bool MenuOutdated = true;
		/// @brief The tabs menu.
		QMenu* TabsMenu;
		/// @brief The dock widget actions buttons.
		QList<tTitleBarButton*> DockWidgetActionsButtons;

		/// @brief The drag start mouse pos.
		QPoint DragStartMousePos;
		/// @brief The drag state.
		eDragState DragState = DraggingInactive;
		/// @brief The floating widget.
		IFloatingWidget* FloatingWidget = nullptr;
};

}// namespace ads

// namespace ads
