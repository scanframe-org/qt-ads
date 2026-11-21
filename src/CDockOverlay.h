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

#include <QFrame>
#include <QHash>
#include <QPointer>
#include <QRect>

#include "ads_globals.h"

QT_FORWARD_DECLARE_CLASS(QGridLayout)

namespace ads
{
struct DockOverlayPrivate;
class CDockOverlayCross;

/**
 * @brief DockOverlay paints a translucent rectangle over another widget. The geometry
 * of the rectangle is based on the mouse location.
 */
class ADS_EXPORT CDockOverlay : public QFrame
{
		Q_OBJECT

	public:
		/**
		 * @brief Alias for the base class.
		 */
		using Super = QFrame;

		/**
		 * @brief Enumeration of the mode values.
		 */
		enum eMode
		{
			ModeDockAreaOverlay,
			ModeContainerOverlay
		};

		/**
		 * @brief Creates a dock overlay.
		 */
		CDockOverlay(QWidget* parent, eMode Mode = ModeDockAreaOverlay);

		/**
		 * @brief Virtual destructor.
		 */
		virtual ~CDockOverlay();

		/**
		 * @brief Configures the areas that are allowed for docking.
		 */
		void setAllowedAreas(DockWidgetAreas areas);

		/**
		 * @brief Enable / disable a certain area.
		 */
		void setAllowedArea(DockWidgetArea area, bool Enable);

		/**
		 * @brief Returns flags with all allowed drop areas.
		 */
		DockWidgetAreas allowedAreas() const;

		/**
		 * @brief Returns the drop area under the current cursor location.
		 */
		DockWidgetArea dropAreaUnderCursor() const;

		/**
		 * @brief Returns the drop area under the given global position.
		 * Use this overload if the position does not come from the mouse cursor
		 * (e.g. from drag and drop events on Wayland, where the global cursor
		 * position is not available).
		 */
		DockWidgetArea dropAreaUnderCursor(const QPoint& GlobalPos) const;

		/**
		 * @brief If the drop area is the CenterDockWidgetArea or a sidebar area,
		 * then this function returns the index of the tab under cursor.
		 * Call this function after call to dropAreaUnderCursor() because this
		 * function updates the tab index.
		 * A value of -1 indicates a position before the first tab and a value of
		 * tabCount() indicates a position behind the last tab.
		 * A value of -2 indicates an valid value.
		 */
		int tabIndexUnderCursor() const;

		/**
		 * @brief This function returns the same like dropAreaUnderCursor() if this
		 * overlay is not hidden and if drop preview is enabled and returns
		 * InvalidDockWidgetArea if it is hidden or drop preview is disabled.
		 */
		DockWidgetArea visibleDropAreaUnderCursor() const;

		/**
		 * @brief This function returns the same like dropAreaUnderCursor(GlobalPos) if
		 * this overlay is not hidden and if drop preview is enabled and returns
		 * InvalidDockWidgetArea if it is hidden or drop preview is disabled.
		 */
		DockWidgetArea visibleDropAreaUnderCursor(const QPoint& GlobalPos) const;

		/**
		 * @brief Show the drop overly for the given target widget.
		 */
		DockWidgetArea showOverlay(QWidget* target);

		/**
		 * @brief Show the drop overlay for the given target widget and evaluate the
		 * drop area for the given global position instead of the cursor position.
		 */
		DockWidgetArea showOverlay(QWidget* target, const QPoint& GlobalPos);

		/**
		 * @brief Hides the overlay.
		 */
		void hideOverlay();

		/**
		 * @brief Enables / disables the semi transparent overlay rectangle that represents
		 * the future area of the dropped widget.
		 */
		void enableDropPreview(bool Enable);

		/**
		 * @brief Returns true if drop preview is enabled.
		 */
		bool dropPreviewEnabled() const;

		/**
		 * @brief The drop overlay rectangle for the target area.
		 */
		QRect dropOverlayRect() const;

		/**
		 * @brief Handle polish events.
		 */
		bool event(QEvent* e) override;

	protected:
		/**
		 * @brief Handles the paint event.
		 */
		void paintEvent(QPaintEvent* e) override;
		/**
		 * @brief Handles the show event.
		 */
		void showEvent(QShowEvent* e) override;
		/**
		 * @brief Handles the hide event.
		 */
		void hideEvent(QHideEvent* e) override;

	private:
		friend struct DockOverlayPrivate;
		friend class CDockOverlayCross;
		/// @brief The d.
		DockOverlayPrivate* d;
};

}// namespace ads
