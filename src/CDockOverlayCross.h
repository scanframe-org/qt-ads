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

#include <QColor>
#include <QHash>
#include <QWidget>

#include "CDockOverlay.h"
#include "ads_globals.h"

namespace ads
{
struct DockOverlayCrossPrivate;

/**
 * @brief DockOverlayCross shows a cross with 5 different drop area possibilities.
 * I could have handled everything inside DockOverlay, but because of some
 * styling issues it's better to have a separate class for the cross.
 * You can style the cross icon using the property system.
 * \code
 * ads--CDockOverlayCross
	{
		qproperty-iconFrameColor: palette(highlight);
		qproperty-iconBackgroundColor: palette(base);
		qproperty-iconOverlayColor: palette(highlight);
		qproperty-iconArrowColor: rgb(227, 227, 227);
		qproperty-iconShadowColor: rgb(0, 0, 0);
	}
 * \endcode
 * Or you can use the iconColors property to pass in AARRGGBB values as
 * hex string like shown in the example below.
 * \code
 * ads--CDockOverlayCross
 * {
 *     qproperty-iconColors: "Frame=#ff3d3d3d Background=#ff929292
 Overlay=#1f3d3d3d Arrow=#ffb4b4b4 Shadow=#40474747";
 * }
 * \endcode
 */
class CDockOverlayCross : public QWidget
{
		Q_OBJECT
		Q_PROPERTY(QString iconColors READ iconColors WRITE setIconColors)
		Q_PROPERTY(QColor iconFrameColor READ iconColor WRITE setIconFrameColor)
		Q_PROPERTY(QColor iconBackgroundColor READ iconColor WRITE setIconBackgroundColor)
		Q_PROPERTY(QColor iconOverlayColor READ iconColor WRITE setIconOverlayColor)
		Q_PROPERTY(QColor iconArrowColor READ iconColor WRITE setIconArrowColor)
		Q_PROPERTY(QColor iconShadowColor READ iconColor WRITE setIconShadowColor)

	public:
		/**
		 * @brief Enumeration of the icon color values.
		 */
		enum eIconColor
		{
			/// @brief The color of the frame of the small window icon.
			FrameColor,
			/// @brief The background color of the small window in the icon.
			WindowBackgroundColor,
			/// @brief The color that shows the overlay (the dock side) in the icon.
			OverlayColor,
			/// @brief The arrow that points into the direction of the icon.
			ArrowColor,
			/// @brief The color of the shadow rectangle that is painted below the icons.
			ShadowColor
		};

		/**
		 * @brief Creates an overlay cross for the given overlay.
		 */
		CDockOverlayCross(CDockOverlay* overlay);

		/**
		 * @brief Virtual destructor.
		 */
		~CDockOverlayCross() override;

	protected:
		/**
		 * @brief This function returns an empty string and is only here to silence
		 * moc.
		 */
		QString iconColors() const;

		/**
		 * @brief This is a dummy function for the property system.
		 */
		QColor iconColor() const
		{
			return QColor();
		}

		/**
		 * @brief Sets the icon frame color.
		 */
		void setIconFrameColor(const QColor& Color)
		{
			setIconColor(FrameColor, Color);
		}

		/**
		 * @brief Sets the icon background color.
		 */
		void setIconBackgroundColor(const QColor& Color)
		{
			setIconColor(WindowBackgroundColor, Color);
		}

		/**
		 * @brief Sets the icon overlay color.
		 */
		void setIconOverlayColor(const QColor& Color)
		{
			setIconColor(OverlayColor, Color);
		}

		/**
		 * @brief Sets the icon arrow color.
		 */
		void setIconArrowColor(const QColor& Color)
		{
			setIconColor(ArrowColor, Color);
		}

		/**
		 * @brief Sets the icon shadow color.
		 */
		void setIconShadowColor(const QColor& Color)
		{
			setIconColor(ShadowColor, Color);
		}

	public:
		/**
		 * @brief Sets a certain icon color.
		 */
		void setIconColor(eIconColor ColorIndex, const QColor& Color);

		/**
		 * @brief Returns the icon color given by ColorIndex.
		 */
		QColor iconColor(eIconColor ColorIndex) const;

		/**
		 * @brief Returns the dock widget area depending on the current cursor location.
		 * The function checks, if the mouse cursor is inside of any drop indicator
		 * widget and returns the corresponding DockWidgetArea.
		 */
		DockWidgetArea cursorLocation() const;

		/**
		 * @brief The function checks, if the given global position is inside of any
		 * drop indicator widget and returns the corresponding DockWidgetArea.
		 */
		DockWidgetArea cursorLocation(const QPoint& GlobalPos) const;

		/**
		 * @brief Sets up the overlay cross for the given overlay mode.
		 */
		void setupOverlayCross(CDockOverlay::eMode Mode);

		/**
		 * @brief Recreates the overlay icons.
		 */
		void updateOverlayIcons();

		/**
		 * @brief Resets and updates the.
		 */
		void reset();

		/**
		 * @brief Updates the current position.
		 */
		void updatePosition();

		/**
		 * @brief A string with all icon colors to set.
		 * You can use this property to style the overly icon via CSS stylesheet
		 * file. The colors are set via a color identifier and a hex AARRGGBB value
		 * like in the example below.
		 * \code
		 * ads--CDockOverlayCross
		 * {
		 *     qproperty-iconColors: "Frame=#ff3d3d3d Background=#ff929292
		 * Overlay=#1f3d3d3d Arrow=#ffb4b4b4 Shadow=#40474747";
		 * }
		 * \endcode
		 */
		void setIconColors(const QString& Colors);

	protected:
		/**
		 * @brief Handles the show event.
		 */
		void showEvent(QShowEvent* e) override;
		/**
		 * @brief Handles generic events.
		 */
		bool event(QEvent* e) override;
		/**
		 * @brief Sets the area widgets.
		 */
		void setAreaWidgets(const QHash<DockWidgetArea, QWidget*>& widgets);

	private:
		friend struct DockOverlayCrossPrivate;
		friend class CDockOverlay;
		/// @brief The d.
		DockOverlayCrossPrivate* d;
};

}// namespace ads
