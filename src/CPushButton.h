#pragma once

/// @file   CPushButton.h
/// @author Uwe Kindler
/// @date   18.10.2022
/// @brief  Declaration of CPushButton.

#include <QPushButton>
#include "ads_globals.h"

namespace ads
{

/**
 * @brief ADS specific push button class with orientation support.
 */
class ADS_EXPORT CPushButton : public QPushButton
{
		Q_OBJECT
	public:
		/**
		 * @brief Enumeration of the orientation values.
		 */
		enum Orientation
		{
			Horizontal,
			VerticalTopToBottom,
			VerticalBottomToTop
		};

		using QPushButton::QPushButton;

		/**
		 * @brief Returns the preferred size of the widget.
		 */
		QSize sizeHint() const override;

		/**
		 * @brief Returns the current orientation.
		 */
		Orientation buttonOrientation() const;

		/**
		 * @brief Set the orientation of this button.
		 */
		void setButtonOrientation(Orientation orientation);

	protected:
		/**
		 * @brief Handles the paint event.
		 */
		void paintEvent(QPaintEvent* event) override;

	private:
		/// @brief The orientation.
		Orientation m_Orientation = Horizontal;
};

}// namespace ads
