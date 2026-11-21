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

/// @file   CDockSplitter.h
/// @author Uwe Kindler
/// @date   24.03.2017
/// @brief  Declaration of CDockSplitter.

#include <QSplitter>

#include "ads_globals.h"

namespace ads
{
struct DockSplitterPrivate;

/**
 * @brief Splitter used internally instead of QSplitter with some additional
 * functionality.
 */
class ADS_EXPORT CDockSplitter : public QSplitter
{
		Q_OBJECT

	public:
		/**
		 * @brief Constructs the object.
		 */
		CDockSplitter(QWidget* parent = Q_NULLPTR);
		/**
		 * @brief Constructs the object.
		 */
		CDockSplitter(Qt::Orientation orientation, QWidget* parent = Q_NULLPTR);

		/**
		 * @brief Prints debug info.
		 */
		~CDockSplitter() override;

		/**
		 * @brief Returns true, if any of the internal widgets is visible.
		 */
		bool hasVisibleContent() const;

		/**
		 * @brief Returns first widget or nullptr if splitter is empty.
		 */
		QWidget* firstWidget() const;

		/**
		 * @brief Returns last widget of nullptr is splitter is empty.
		 */
		QWidget* lastWidget() const;

		/**
		 * @brief Returns true if the splitter contains central widget of dock manager.
		 */
		bool isResizingWithContainer() const;

	private:
		friend struct DockSplitterPrivate;
		/// @brief The d.
		DockSplitterPrivate* d;
};

}// namespace ads
