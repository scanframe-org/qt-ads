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

/// @file   CFloatingWidgetTitleBar_p.h
/// @author Uwe Kindler
/// @date   13.05.2019
/// @brief  Private data of CFloatingWidgetTitleBar class.

#include "CFloatingWidgetTitleBar.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QStyle>
#include <QToolButton>

#include <iostream>

#include "CElidingLabel.h"
#include "CFloatingDockContainer.h"
#include "ads_globals.h"

namespace ads
{

using tTabLabel = CElidingLabel;
/**
 * @brief Alias type for close button.
 */
using tCloseButton = QToolButton;
/**
 * @brief Alias type for maximize button.
 */
using tMaximizeButton = QToolButton;

/**
 * @brief Private data class of public interface CFloatingWidgetTitleBar.
 */
struct FloatingWidgetTitleBarPrivate
{

		/**
		 * @brief Constructs the object.
		 */
		FloatingWidgetTitleBarPrivate(CFloatingWidgetTitleBar* _public)
			: _this(_public)
		{}

		/**
		 * @brief Creates the complete layout including all controls.
		 */
		void createLayout();
		/// Public interface class.
		CFloatingWidgetTitleBar* _this;
		/// @brief The icon label.
		QLabel* IconLabel = nullptr;
		/// @brief The title label.
		tTabLabel* TitleLabel;
		/// @brief The close button.
		tCloseButton* CloseButton = nullptr;
		/// @brief The maximize button.
		tMaximizeButton* MaximizeButton = nullptr;
		/// @brief The floating widget.
		CFloatingDockContainer* FloatingWidget = nullptr;
		/// @brief The drag state.
		eDragState DragState = DraggingInactive;
		/// @brief The maximize icon.
		QIcon MaximizeIcon;
		/// @brief The normal icon.
		QIcon NormalIcon;
		/// @brief The maximized.
		bool Maximized = false;
};
}// namespace ads
