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

/// @file   CDockContainerWidget_p.h
/// @author Uwe Kindler
/// @date   24.02.2017
/// @brief  Private data of CDockContainerWidget class.

#include "CDockContainerWidget.h"

#include <QAbstractButton>
#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QList>
#include <QMetaObject>
#include <QMetaType>
#include <QMimeData>
#include <QPointer>
#include <QTimer>
#include <QVariant>
#include <QXmlStreamWriter>

#include <functional>
#include <iostream>

#include "CAutoHideDockContainer.h"
#include "CAutoHideSideBar.h"
#include "CAutoHideTab.h"
#include "CDockAreaTitleBar.h"
#include "CDockAreaWidget.h"
#include "CDockFocusController.h"
#include "CDockManager.h"
#include "CDockOverlay.h"
#include "CDockSplitter.h"
#include "CDockWidget.h"
#include "CDockWidgetTab.h"
#include "CDockingStateReader.h"
#include "CFloatingDockContainer.h"
#include "ads_globals.h"

#if QT_VERSION < 0x050900

/**
 * @brief Implements to hex lower.
 */
inline char toHexLower(uint value)
{
	return "0123456789abcdef"[value & 0xF];
}

/**
 * @brief Implements q byte array to hex.
 */
QByteArray qByteArrayToHex(const QByteArray& src, char separator)
{
	if (src.size() == 0)
		return QByteArray();

	const int length = separator ? (src.size() * 3 - 1) : (src.size() * 2);
	QByteArray hex(length, Qt::Uninitialized);
	char* hexData = hex.data();
	const uchar* data = reinterpret_cast<const uchar*>(src.data());
	for (int i = 0, o = 0; i < src.size(); ++i)
	{
		hexData[o++] = toHexLower(data[i] >> 4);
		hexData[o++] = toHexLower(data[i] & 0xf);

		if ((separator) && (o < length))
			hexData[o++] = separator;
	}
	return hex;
}
#endif

namespace ads
{
/**
 * @brief The z order counter.
 */
static unsigned int zOrderCounter = 0;

/**
 * @brief Enumeration of the drop mode values.
 */
enum eDropMode
{
	/// Drop widget into a dock area.
	DropModeIntoArea,
	/// Drop into container.
	DropModeIntoContainer,
	/// Invalid mode - do not drop.
	DropModeInvalid
};

/**
 * @brief Converts dock area ID to an index for array access.
 */
static int areaIdToIndex(DockWidgetArea area)
{
	switch (area)
	{
		case LeftDockWidgetArea:
			return 0;
		case RightDockWidgetArea:
			return 1;
		case TopDockWidgetArea:
			return 2;
		case BottomDockWidgetArea:
			return 3;
		case CenterDockWidgetArea:
			return 4;
		default:
			return 4;
	}
}

/**
 * @brief Helper function to ease insertion of dock area into splitter.
 */
static void insertWidgetIntoSplitter(QSplitter* Splitter, QWidget* widget, bool Append)
{
	if (Append)
	{
		Splitter->addWidget(widget);
	}
	else
	{
		Splitter->insertWidget(0, widget);
	}
}

/**
 * @brief Private data class of CDockContainerWidget class (pimpl).
 */
class DockContainerWidgetPrivate
{
	public:
		/**
		 * @brief Private data constructor.
		 */
		DockContainerWidgetPrivate(CDockContainerWidget* _public);

		/**
		 * @brief Adds dock widget to container and returns the dock area that contains
		 * the inserted dock widget.
		 */
		CDockAreaWidget* addDockWidgetToContainer(DockWidgetArea area, CDockWidget* Dockwidget);

		/**
		 * @brief Adds dock widget to a existing DockWidgetArea.
		 */
		CDockAreaWidget* addDockWidgetToDockArea(DockWidgetArea area, CDockWidget* Dockwidget, CDockAreaWidget* TargetDockArea, int Index = -1);

		/**
		 * @brief Add dock area to this container.
		 */
		void addDockArea(CDockAreaWidget* NewDockWidget, DockWidgetArea area = CenterDockWidgetArea);

		/**
		 * @brief Drop floating widget into container.
		 */
		void dropIntoContainer(CFloatingDockContainer* FloatingWidget, DockWidgetArea area);

		/**
		 * @brief Drop floating widget into auto hide side bar.
		 */
		void dropIntoAutoHideSideBar(CFloatingDockContainer* FloatingWidget, DockWidgetArea area);

		/**
		 * @brief Shows the drop overlays for the given floating widget that is dragged
		 * over this container by a drag and drop operation (Wayland platform
		 * drag).
		 */
		void updateDropOverlays(const QPoint& GlobalPos, CFloatingDockContainer* FloatingWidget);

		/**
		 * @brief Creates a new tab for a widget dropped into the center of a section.
		 */
		void dropIntoCenterOfSection(CFloatingDockContainer* FloatingWidget, CDockAreaWidget* TargetArea, int TabIndex = 0);

		/**
		 * @brief Drop floating widget into dock area.
		 */
		void dropIntoSection(CFloatingDockContainer* FloatingWidget, CDockAreaWidget* TargetArea, DockWidgetArea area, int TabIndex = 0);

		/**
		 * @brief Moves the dock widget or dock area given in Widget parameter to a
		 * new dock widget area.
		 */
		void moveToNewSection(QWidget* Widget, CDockAreaWidget* TargetArea, DockWidgetArea area, int TabIndex = 0);

		/**
		 * @brief Moves the dock widget or dock area given in Widget parameter to a
		 * a dock area in container.
		 */
		void moveToContainer(QWidget* Widgett, DockWidgetArea area);

		/**
		 * @brief Creates a new tab for a widget dropped into the center of a section.
		 */
		void moveIntoCenterOfSection(QWidget* Widget, CDockAreaWidget* TargetArea, int TabIndex = 0);

		/**
		 * @brief Moves the dock widget or dock area given in Widget parameter to
		 * a auto hide sidebar area.
		 */
		void moveToAutoHideSideBar(QWidget* Widget, DockWidgetArea area, int TabIndex = TabDefaultInsertIndex);

		/**
		 * @brief Adds new dock areas to the internal dock area list.
		 */
		void addDockAreasToList(const QList<CDockAreaWidget*> NewDockAreas);

		/**
		 * @brief Wrapper function for DockAreas append, that ensures that dock area signals
		 * are properly connected to dock container slots.
		 */
		void appendDockAreas(const QList<CDockAreaWidget*> NewDockAreas);

		/**
		 * @brief Save state of child nodes.
		 */
		void saveChildNodesState(QXmlStreamWriter& Stream, QWidget* Widget);

		/**
		 * @brief Save state of auto hide widgets.
		 */
		void saveAutoHideWidgetsState(QXmlStreamWriter& Stream);

		/**
		 * @brief Restore state of child nodes.
		 * @param[in] Stream The data stream that contains the serialized state.
		 * @param[out] CreatedWidget The widget created from parsed data or 0 if
		 * the parsed widget was an empty splitter.
		 * @param[in] Testing If Testing is true, only the stream data is
		 * parsed without modifying anything.
		 */
		bool restoreChildNodes(CDockingStateReader& Stream, QWidget*& CreatedWidget, bool Testing);

		/**
		 * @brief Restores a splitter.
		 * @see restoreChildNodes() for details
		 */
		bool restoreSplitter(CDockingStateReader& Stream, QWidget*& CreatedWidget, bool Testing);

		/**
		 * @brief Restores a dock area.
		 * @see restoreChildNodes() for details
		 */
		bool restoreDockArea(CDockingStateReader& Stream, QWidget*& CreatedWidget, bool Testing);

		/**
		 * @brief Restores a auto hide side bar.
		 */
		bool restoreSideBar(CDockingStateReader& Stream, QWidget*& CreatedWidget, bool Testing);

		/**
		 * @brief Helper function for recursive dumping of layout.
		 */
		void dumpRecursive(int level, QWidget* widget);

		/**
		 * @brief Calculate the drop mode from the given target position.
		 */
		eDropMode getDropMode(const QPoint& TargetPos);

		/**
		 * @brief Initializes the visible dock area count variable if it is not initialized
		 * yet.
		 */
		void initVisibleDockAreaCount()
		{
			if (VisibleDockAreaCount > -1)
			{
				return;
			}

			VisibleDockAreaCount = 0;
			for (auto DockArea: DockAreas)
			{
				if (!DockArea)
				{
					continue;
				}
				VisibleDockAreaCount += (DockArea->isHidden() ? 0 : 1);
			}
		}

		/**
		 * @brief Access function for the visible dock area counter.
		 */
		int& visibleDockAreaCount()
		{
			// Lazy initialisation - we initialize the VisibleDockAreaCount variable
			// on first use
			initVisibleDockAreaCount();
			return VisibleDockAreaCount;
		}

		/**
		 * @brief The visible dock area count changes, if dock areas are remove, added or
		 * when its view is toggled.
		 */
		void onVisibleDockAreaCountChanged();

		/**
		 * @brief Signal emitted when emit dock areas removed.
		 */
		void emitDockAreasRemoved()
		{
			onVisibleDockAreaCountChanged();
			Q_EMIT _this->dockAreasRemoved();
		}

		/**
		 * @brief Signal emitted when emit dock areas added.
		 */
		void emitDockAreasAdded()
		{
			onVisibleDockAreaCountChanged();
			Q_EMIT _this->dockAreasAdded();
		}

		/**
		 * @brief Helper function for creation of new splitter.
		 */
		CDockSplitter* newSplitter(Qt::Orientation orientation, QWidget* parent = nullptr)
		{
			CDockSplitter* s = new CDockSplitter(orientation, parent);
			s->setOpaqueResize(CDockManager::testConfigFlag(CDockManager::OpaqueSplitterResize));
			s->setChildrenCollapsible(false);
			return s;
		}

		/**
		 * @brief Ensures equal distribution of the sizes of a splitter if an dock widget
		 * is inserted from code.
		 */
		void adjustSplitterSizesOnInsertion(QSplitter* Splitter, qreal LastRatio = 1.0)
		{
			int AreaSize = (Splitter->orientation() == Qt::Horizontal) ? Splitter->width() : Splitter->height();
			auto SplitterSizes = Splitter->sizes();

			qreal TotRatio = SplitterSizes.size() - 1.0 + LastRatio;
			for (int i = 0; i < SplitterSizes.size() - 1; i++)
			{
				SplitterSizes[i] = AreaSize / TotRatio;
			}
			SplitterSizes.back() = AreaSize * LastRatio / TotRatio;
			Splitter->setSizes(SplitterSizes);
		}

		/**
		 * @brief This function forces the dock container widget to update handles of
		 * splitters based if a central widget exists.
		 */
		void updateSplitterHandles(QSplitter* splitter);

		/**
		 * @brief If no central widget exists, the widgets resize with the container.
		 * If a central widget exists, the widgets surrounding the central widget
		 * do not resize its height or width.
		 */
		bool widgetResizesWithContainer(QWidget* widget);

		// private slots: ------------------------------------------------------------
		/**
		 * @brief Slot invoked on dock area view toggled.
		 */
		void onDockAreaViewToggled(bool Visible)
		{
			CDockAreaWidget* DockArea = qobject_cast<CDockAreaWidget*>(_this->sender());
			VisibleDockAreaCount += Visible ? 1 : -1;
			onVisibleDockAreaCountChanged();
			Q_EMIT _this->dockAreaViewToggled(DockArea, Visible);
		}

		/// @brief The this.
		CDockContainerWidget* _this;
		/// @brief The dock manager.
		QPointer<CDockManager> DockManager;
		/// @brief The z order index.
		unsigned int zOrderIndex = 0;
		/// @brief The dock areas.
		QList<QPointer<CDockAreaWidget>> DockAreas;
		/// @brief The auto hide widgets.
		QList<CAutoHideDockContainer*> AutoHideWidgets;
		/// @brief The side tab bar widgets.
		QMap<SideBarLocation, CAutoHideSideBar*> SideTabBarWidgets;
		/// @brief The layout.
		QGridLayout* Layout = nullptr;
		/// @brief The root splitter.
		CDockSplitter* RootSplitter = nullptr;
		/// @brief The is floating.
		bool isFloating = false;
		/// @brief The last added area cache[5].
		CDockAreaWidget* LastAddedAreaCache[5];
		/// @brief The visible dock area count.
		int VisibleDockAreaCount = -1;
		/// @brief The top level dock area.
		CDockAreaWidget* TopLevelDockArea = nullptr;
		/// @brief The delayed auto hide timer.
		QTimer DelayedAutoHideTimer;
		/// @brief The delayed auto hide tab.
		QPointer<CAutoHideTab> DelayedAutoHideTab;
		/// @brief The delayed auto hide show.
		bool DelayedAutoHideShow = false;
};
}// namespace ads
