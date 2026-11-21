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

/// @file   CFloatingWidgetTitleBar.h
/// @author Uwe Kindler
/// @date   13.05.2019
/// @brief  Declaration of CFloatingWidgetTitleBar class.

#include <QFrame>
#include <QIcon>

namespace ads
{
class CFloatingDockContainer;

struct FloatingWidgetTitleBarPrivate;

/**
 * @brief Titlebar for floating widgets to capture non client are mouse events.
 * Linux does not support NonClieantArea mouse events like
 * QEvent::NonClientAreaMouseButtonPress. Because these events are required
 * for the docking system to work properly, we use our own titlebar here to
 * capture the required mouse events.
 */
class CFloatingWidgetTitleBar : public QFrame
{
		Q_OBJECT
		Q_PROPERTY(QIcon maximizeIcon READ maximizeIcon WRITE setMaximizeIcon)
		Q_PROPERTY(QIcon normalIcon READ normalIcon WRITE setNormalIcon)

	public:
		/**
		 * @brief Constructs the object.
		 */
		explicit CFloatingWidgetTitleBar(CFloatingDockContainer* parent = nullptr);

		/**
		 * @brief Virtual Destructor.
		 */
		~CFloatingWidgetTitleBar() override;

	protected:
		/**
		 * @brief Handles the mouse press event.
		 */
		void mousePressEvent(QMouseEvent* ev) override;

		/**
		 * @brief Handles the mouse release event.
		 */
		void mouseReleaseEvent(QMouseEvent* ev) override;

		/**
		 * @brief Handles the mouse move event.
		 */
		void mouseMoveEvent(QMouseEvent* ev) override;

		/**
		 * @brief Handles the mouse double click event.
		 */
		void mouseDoubleClickEvent(QMouseEvent* event) override;

		/**
		 * @brief Returns the preferred size of the widget.
		 */
		QSize sizeHint() const override;

		/**
		 * @brief Sets the maximize icon.
		 */
		void setMaximizeIcon(const QIcon& Icon);

		/**
		 * @brief Returns the maximize icon.
		 */
		QIcon maximizeIcon() const;

		/**
		 * @brief Sets the normal icon.
		 */
		void setNormalIcon(const QIcon& Icon);

		/**
		 * @brief Returns the normal icon.
		 */
		QIcon normalIcon() const;

	public:
		/**
		 * @brief Alias for the base class.
		 */
		using Super = QWidget;

		/**
		 * @brief Enables / disables the window close button.
		 */
		void enableCloseButton(bool Enable);

		/**
		 * @brief Sets the window title, that means, the text of the internal tile label.
		 */
		void setTitle(const QString& Text);

		/**
		 * @brief Update stylesheet style if a property changes.
		 */
		void updateStyle();

		/**
		 * @brief Change the maximize button icon according to current windows state.
		 */
		void setMaximizedIcon(bool maximized);

	signals:
		/**
		 * @brief This signal is emitted, if the close button is clicked.
		 */
		void closeRequested();

		/**
		 * @brief This signal is emitted, if the maximize button is clicked.
		 */
		void maximizeRequested();

	private:
		/// Private data (pimpl).
		FloatingWidgetTitleBarPrivate* d;
};
}// namespace ads
