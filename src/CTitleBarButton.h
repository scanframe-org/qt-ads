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

#include <QToolButton>
#include "ads_globals.h"

namespace ads
{
class CDockAreaTitleBar;

using tTitleBarButton = QToolButton;

/**
 * @brief Title bar button of a dock area that customizes tTitleBarButton
 * appearance/behavior according to various config flags such as:
 * CDockManager::DockAreaHas_xxx_Button - if set to 'false' keeps the button
 * always invisible CDockManager::DockAreaHideDisabledButtons - if set to 'true'
 * hides button when it is disabled.
 */
class CTitleBarButton : public tTitleBarButton
{
		Q_OBJECT

	public:
		/**
		 * @brief Constructs the object.
		 */
		CTitleBarButton(bool ShowInTitleBar, bool HideWhenDisabled, TitleBarButton ButtonId, QWidget* parent = nullptr);
		/**
		 * @brief Alias for the base class.
		 */
		using Super = tTitleBarButton;

		/**
		 * @brief Adjust this visibility change request with our internal settings:.
		 */
		void setVisible(bool visible) override;

		/**
		 * @brief Configures, if the title bar button should be shown in title bar.
		 */
		void setShowInTitleBar(bool Show);

		/**
		 * @brief Identifier for the title bar button.
		 */
		TitleBarButton buttonId() const;

		/**
		 * @brief Return the title bar that contains this button.
		 */
		CDockAreaTitleBar* titleBar() const;

		/**
		 * @brief Returns true, if the button is in a title bar in an auto hide area.
		 */
		bool isInAutoHideArea() const;

	protected:
		/**
		 * @brief Handle EnabledChanged signal to set button invisible if the configured.
		 */
		bool event(QEvent* ev) override;

	private:
		/// @brief The show in title bar.
		bool ShowInTitleBar = true;
		/// @brief The hide when disabled.
		bool HideWhenDisabled = false;
		/// @brief The title bar button id.
		TitleBarButton TitleBarButtonId;
};

}// namespace ads
