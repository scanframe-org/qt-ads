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

/// @file   CDockManager_p.h
/// @author Uwe Kindler
/// @date   26.02.2017
/// @brief  Private data of CDockManager class.

#include "CDockManager.h"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QDialog>
#include <QFile>
#include <QList>
#include <QMainWindow>
#include <QMap>
#include <QMenu>
#include <QSettings>
#include <QStyleHints>
#include <QVariant>
#include <QVector>
#include <QWindow>
#include <QWindowStateChangeEvent>
#include <QXmlStreamWriter>

#include <CAutoHideDockContainer.h>

#include <algorithm>
#include <iostream>

#include "CDockAreaTitleBar.h"
#include "CDockAreaWidget.h"
#include "CDockComponentsFactory.h"
#include "CDockFocusController.h"
#include "CDockOverlay.h"
#include "CDockSplitter.h"
#include "CDockWidget.h"
#include "CDockWidgetTab.h"
#include "CDockingStateReader.h"
#include "CFloatingDockContainer.h"
#include "CIconProvider.h"
#include "ads_globals.h"

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
	#include "linux/CFloatingWidgetTitleBar.h"
#endif

namespace ads
{
/**
 * @brief Internal file version in case the structure changes internally.
 */
enum eStateFileVersion
{
	// !< InitialVersion
	InitialVersion = 0,
	// !< Version1
	Version1 = 1,
	// !< CurrentVersion
	CurrentVersion = Version1
};

/**
 * @brief The static config flags.
 */
static CDockManager::ConfigFlags StaticConfigFlags = CDockManager::DefaultNonOpaqueConfig;
// auto hide feature is disabled by default
/**
 * @brief The static auto hide config flags.
 */
static CDockManager::AutoHideFlags StaticAutoHideConfigFlags;
/**
 * @brief Implements static config params.
 */
static QVector<QVariant> StaticConfigParams(CDockManager::ConfigParamCount);

/**
 * @brief The floating containers title.
 */
static QString FloatingContainersTitle;

/**
 * @brief Private data class of CDockManager class (pimpl).
 */
struct DockManagerPrivate
{

		/**
		 * @brief Private data constructor.
		 */
		DockManagerPrivate(CDockManager* _public);

		/**
		 * @brief Checks if the given data stream is a valid docking system state
		 * file.
		 */
		bool checkFormat(const QByteArray& state, int version);

		/**
		 * @brief Restores the state.
		 */
		bool restoreStateFromXml(const QByteArray& state, int version, bool Testing = internal::Restore);

		/**
		 * @brief Restore state.
		 */
		bool restoreState(const QByteArray& state, int version);

		/**
		 * @brief Restores dock widgets open state.
		 */
		void restoreDockWidgetsOpenState();
		/**
		 * @brief Restores dock areas indices.
		 */
		void restoreDockAreasIndices();
		/**
		 * @brief Emits top level events.
		 */
		void emitTopLevelEvents();

		/**
		 * @brief Hides floating widgets.
		 */
		void hideFloatingWidgets()
		{
			// Hide updates of floating widgets from user
			for (auto FloatingWidget: FloatingWidgets)
			{
				if (FloatingWidget)
				{
					FloatingWidget->hide();
				}
			}
		}

		/**
		 * @brief Mark dock widgets dirty.
		 */
		void markDockWidgetsDirty()
		{
			for (auto DockWidget: DockWidgetsMap)
			{
				DockWidget->setProperty(internal::DirtyProperty, true);
			}
		}

		/**
		 * @brief Restores the container with the given index.
		 */
		bool restoreContainer(int Index, CDockingStateReader& stream, bool Testing);

		/**
		 * @brief Loads the stylesheet.
		 */
		void loadStylesheet();

		/**
		 * @brief Adds action to menu - optionally in sorted order.
		 */
		void addActionToMenu(QAction* Action, QMenu* Menu, bool InsertSorted);
		/// @brief The this.
		CDockManager* _this;
		/// @brief The floating widgets.
		QList<QPointer<CFloatingDockContainer>> FloatingWidgets;
		/// @brief The hidden floating widgets.
		QList<QPointer<CFloatingDockContainer>> HiddenFloatingWidgets;
		/// @brief The containers.
		QList<CDockContainerWidget*> Containers;
		/// @brief The container overlay.
		CDockOverlay* ContainerOverlay;
		/// @brief The dock area overlay.
		CDockOverlay* DockAreaOverlay;
		/// @brief The dock widgets map.
		QMap<QString, CDockWidget*> DockWidgetsMap;
		/// @brief The perspectives.
		QMap<QString, QByteArray> Perspectives;
		/// @brief The view menu groups.
		QMap<QString, QMenu*> ViewMenuGroups;
		/// @brief The view menu.
		QMenu* ViewMenu;
		/// @brief The menu insertion order.
		CDockManager::eViewMenuInsertionOrder MenuInsertionOrder = CDockManager::MenuAlphabeticallySorted;
		/// @brief The color scheme mode.
		CDockManager::ColorSchemeMode ColorSchemeMode = CDockManager::ColorSchemeMode::FollowPalette;
		/// @brief The restoring state.
		bool RestoringState = false;
		/// @brief The uninitialized floating widgets.
		QVector<CFloatingDockContainer*> UninitializedFloatingWidgets;
		/// @brief The focus controller.
		CDockFocusController* FocusController = nullptr;
		/// @brief The restored focused dock widget.
		QString RestoredFocusedDockWidget;
		/// @brief The central widget.
		CDockWidget* CentralWidget = nullptr;
		/// @brief The is leaving minimized.
		bool IsLeavingMinimized = false;
		/// @brief The tool bar style docked.
		Qt::ToolButtonStyle ToolBarStyleDocked = Qt::ToolButtonIconOnly;
		/// @brief The tool bar style floating.
		Qt::ToolButtonStyle ToolBarStyleFloating = Qt::ToolButtonTextUnderIcon;
		/// @brief The tool bar icon size docked.
		QSize ToolBarIconSizeDocked = QSize(16, 16);
		/// @brief The tool bar icon size floating.
		QSize ToolBarIconSizeFloating = QSize(24, 24);
		/// @brief The locked dock widget features.
		CDockWidget::DockWidgetFeatures LockedDockWidgetFeatures;
		/// @brief The component factory.
		QSharedPointer<ads::CDockComponentsFactory> ComponentFactory{ads::CDockComponentsFactory::factory()};
		/// @brief The current stylesheet dark.
		bool CurrentStylesheetDark;
};
}// namespace ads
