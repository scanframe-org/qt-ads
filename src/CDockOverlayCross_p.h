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

#include "CDockOverlayCross.h"
#include "CDockOverlay_p.h"

namespace ads
{

/**
 * @brief Private data of CDockOverlayCross class.
 */
struct DockOverlayCrossPrivate
{

		/**
		 * @brief Private data constructor.
		 */
		DockOverlayCrossPrivate(CDockOverlayCross* _public);

		/**
		 * @brief Returns the grid position of the given area.
		 * @param area Dock widget area.
		 * @return Position in the overlay grid layout.
		 */
		QPoint areaGridPosition(const DockWidgetArea area);

		/**
		 * @brief Palette based default icon colors.
		 */
		QColor defaultIconColor(CDockOverlayCross::eIconColor ColorIndex);

		/**
		 * @brief Stylehseet based icon colors.
		 */
		QColor iconColor(CDockOverlayCross::eIconColor ColorIndex);

		/**
		 * @brief Helper function that returns the drop indicator width depending on the
		 * operating system.
		 */
		qreal dropIndicatiorWidth(QLabel* l) const;

		/**
		 * @brief Creates drop indicator widget.
		 */
		QWidget* createDropIndicatorWidget(DockWidgetArea DockWidgetArea, CDockOverlay::eMode Mode);

		/**
		 * @brief Updates drop indicator icon.
		 */
		void updateDropIndicatorIcon(QWidget* DropIndicatorWidget);

		/**
		 * @brief Creates high dpi drop indicator pixmap.
		 */
		QPixmap createHighDpiDropIndicatorPixmap(const QSizeF& size, DockWidgetArea DockWidgetArea, CDockOverlay::eMode Mode);

		/// @brief The this.
		CDockOverlayCross* _this;
		/// @brief The mode.
		CDockOverlay::eMode Mode = CDockOverlay::ModeDockAreaOverlay;
		/// @brief The dock overlay.
		CDockOverlay* DockOverlay;
		/// @brief The drop indicator widgets.
		QHash<DockWidgetArea, QWidget*> DropIndicatorWidgets;
		/// @brief The grid layout.
		QGridLayout* GridLayout;
		/// @brief The icon colors[5].
		QColor IconColors[5];
		/// @brief The update required.
		bool UpdateRequired = false;
		/// @brief The last device pixel ratio.
		double LastDevicePixelRatio = 0.1;
};
}// namespace ads
