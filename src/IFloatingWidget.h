#pragma once

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

#include "ads_globals.h"

QT_FORWARD_DECLARE_CLASS(QWidget)

namespace ads
{

/**
 * @brief Pure virtual interface for floating widgets.
 * This interface is used for opaque and non-opaque undocking. If opaque
 * undocking is used, the real CFloatingDockContainer widget will be created.
 */
class ADS_EXPORT IFloatingWidget
{
	public:
		/**
		 * @brief Constructs the floating widget interface.
		 */
		virtual ~IFloatingWidget() = default;

		/**
		 * @brief Starts floating.
		 * This function should get called typically from a mouse press event
		 * handler.
		 */
		virtual void startFloating(const QPoint& DragStartMousePos, const QSize& Size, eDragState DragState, QWidget* MouseEventHandler) = 0;

		/**
		 * @brief Moves the widget to a new position relative to the position given when
		 * startFloating() was called.
		 * This function should be called from a mouse mouve event handler to
		 * move the floating widget on mouse move events.
		 */
		virtual void moveFloating() = 0;

		/**
		 * @brief Tells the widget that to finish dragging if the mouse is released.
		 * This function should be called from a mouse release event handler
		 * to finish the dragging.
		 */
		virtual void finishDragging() = 0;
};

}// namespace ads
