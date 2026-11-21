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

#include <QWidget>

namespace ads
{

/**
 * @brief This spacer widget is here because of the following problem.
 * The dock area title bar handles mouse dragging and moving the floating widget.
 * The  problem is, that if the title bar becomes invisible, i.e. if the dock
 * area contains only one single dock widget and the dock area is moved
 * into a floating widget, then mouse events are not handled anymore and dragging
 * of the floating widget stops.
 */
class CSpacerWidget : public QWidget
{
		Q_OBJECT
	public:
		/**
		 * @brief Alias for the base class.
		 */
		using Super = QWidget;
		/**
		 * @brief Constructs the object.
		 */
		CSpacerWidget(QWidget* Parent = nullptr);

		/**
		 * @brief Returns the preferred size of the widget.
		 */
		QSize sizeHint() const override
		{
			return QSize(0, 0);
		}

		/**
		 * @brief Returns the minimum size hint of the widget.
		 */
		QSize minimumSizeHint() const override
		{
			return QSize(0, 0);
		}
};

}// namespace ads
