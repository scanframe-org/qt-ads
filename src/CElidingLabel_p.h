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

/// @file   CElidingLabel_p.h
/// @author Uwe Kindler
/// @date   05.11.2018
/// @brief  Private data of CElidingLabel.

#include "CElidingLabel.h"

#include <QMouseEvent>

namespace ads
{
/**
 * @brief Private data of public CClickableLabel.
 */
struct ElidingLabelPrivate
{

		/**
		 * @brief Constructs the object.
		 */
		ElidingLabelPrivate(CElidingLabel* _public);

		/**
		 * @brief Elides text.
		 */
		void elideText(int Width);

		/**
		 * @brief Convenience function to check if the.
		 */
		bool isModeElideNone() const;

		/// @brief The this.
		CElidingLabel* _this;
		/// @brief The elide mode.
		Qt::TextElideMode ElideMode = Qt::ElideNone;
		/// @brief The text.
		QString Text;
		/// @brief The is elided.
		bool IsElided = false;
};
}// namespace ads
