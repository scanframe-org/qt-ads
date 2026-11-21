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

/// @file   CDockSplitter_p.h
/// @author Uwe Kindler
/// @date   24.03.2017
/// @brief  Private data of CDockSplitter.

#include "CDockSplitter.h"

namespace ads
{
/**
 * @brief Private dock splitter data.
 */
struct DockSplitterPrivate
{

		/**
		 * @brief Constructs the object.
		 */
		DockSplitterPrivate(CDockSplitter* _public);

		/// @brief The this.
		CDockSplitter* _this;
		/// @brief The visible content count.
		int VisibleContentCount = 0;
};

inline DockSplitterPrivate::DockSplitterPrivate(CDockSplitter* _public)
	: _this(_public)
{}
}// namespace ads
