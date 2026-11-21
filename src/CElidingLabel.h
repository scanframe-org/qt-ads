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

/// @file   CElidingLabel.h
/// @author Uwe Kindler
/// @date   05.11.2018
/// @brief  Declaration of CElidingLabel.

#include <QLabel>

#include "ads_globals.h"

namespace ads
{
struct ElidingLabelPrivate;

/**
 * @brief A QLabel that supports eliding text.
 * Because the functions setText() and text() are no virtual functions setting
 * and reading the text via a pointer to the base class QLabel does not work
 * properly.
 */
class ADS_EXPORT CElidingLabel : public QLabel
{
		Q_OBJECT

	public:
		/**
		 * @brief Constructs the object.
		 */
		CElidingLabel(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
		/**
		 * @brief Constructs the object.
		 */
		CElidingLabel(const QString& text, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
		/**
		 * @brief Constructs the object.
		 */
		virtual ~CElidingLabel();

	protected:
		/**
		 * @brief Handles the mouse release event.
		 */
		void mouseReleaseEvent(QMouseEvent* event) override;
		/**
		 * @brief Handles the resize event.
		 */
		void resizeEvent(QResizeEvent* event) override;
		/**
		 * @brief Handles the mouse double click event.
		 */
		void mouseDoubleClickEvent(QMouseEvent* ev) override;

	public:
		/**
		 * @brief Alias for the base class.
		 */
		using Super = QLabel;

		/**
		 * @brief Returns the text elide mode.
		 * The default mode is ElideNone.
		 */
		Qt::TextElideMode elideMode() const;

		/**
		 * @brief Sets the text elide mode.
		 */
		void setElideMode(Qt::TextElideMode mode);

		/**
		 * @brief This function indicates whether the text on this label is currently elided.
		 */
		bool isElided() const;

	public:// reimplements QLabel ----------------------------------------------
		/**
		 * @brief Returns the minimum size hint of the widget.
		 */
		QSize minimumSizeHint() const override;
		/**
		 * @brief Returns the preferred size of the widget.
		 */
		QSize sizeHint() const override;
		/**
		 * @brief Sets the text.
		 */
		void setText(const QString& text);
		/**
		 * @brief Returns the text.
		 */
		QString text() const;

	Q_SIGNALS:
		/**
		 * @brief This signal is emitted if the user clicks on the label (i.e. pressed
		 * down then released while the mouse cursor is inside the label).
		 */
		void clicked();

		/**
		 * @brief This signal is emitted if the user does a double click on the label.
		 */
		void doubleClicked();

		/**
		 * @brief This signal is emitted when isElided() state of this label is changed.
		 */
		void elidedChanged(bool elided);

	private:
		friend struct ElidingLabelPrivate;
		/// @brief The d.
		ElidingLabelPrivate* d;
};

}// namespace ads
