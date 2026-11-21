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

/// @file   CDockWidgetTab_p.h
/// @author Uwe Kindler
/// @date   27.02.2017
/// @brief  Private data of CDockWidgetTab class.

#include "CDockWidgetTab.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QStyle>
#include <QToolButton>
#include "CDockAreaWidget.h"
#include "CDockManager.h"
#include "CDockWidget.h"
#include "CElidingLabel.h"
#include "CFloatingDockContainer.h"
#include "CFloatingDragPreview.h"
#include "ads_globals.h"

namespace ads
{
/**
 * @brief Alias type for tab label.
 */
using tTabLabel = CElidingLabel;

/**
 * @brief Private data class of CDockWidgetTab class (pimpl).
 */
struct DockWidgetTabPrivate
{

		/**
		 * @brief Private data constructor.
		 */
		DockWidgetTabPrivate(CDockWidgetTab* _public);

		/**
		 * @brief Creates the complete layout including all controls.
		 */
		void createLayout();

		/**
		 * @brief Moves the tab depending on the position in the given mouse event.
		 */
		void moveTab(QMouseEvent* ev);

		/**
		 * @brief Test function for current drag state.
		 */
		bool isDraggingState(eDragState dragState) const
		{
			return this->DragState == dragState;
		}

		/**
		 * @brief Starts floating of the dock widget that belongs to this title bar
		 * Returns true, if floating has been started and false if floating
		 * is not possible for any reason.
		 */
		bool startFloating(eDragState DraggingState = DraggingFloatingWidget);

		/**
		 * @brief Wayland hybrid drag: drive the in-window drag preview from reliable
		 * event coordinates and, when the cursor leaves the source top-level
		 * window, convert the in-window drag into a native compositor platform
		 * drag.
		 */
		void waylandPreviewMove(QMouseEvent* ev);

		/**
		 * @brief Returns true if the given config flag is set.
		 */
		bool testConfigFlag(CDockManager::eConfigFlag Flag) const
		{
			return CDockManager::testConfigFlag(Flag);
		}

		/**
		 * @brief Creates the close button as QPushButton or as QToolButton.
		 */
		QAbstractButton* createCloseButton() const
		{
			if (testConfigFlag(CDockManager::TabCloseButtonIsToolButton))
			{
				auto Button = new QToolButton();
				Button->setAutoRaise(true);
				return Button;
			}
			else
			{
				return new QPushButton();
			}
		}

		/**
		 * @brief Update the close button visibility from current feature/config.
		 */
		void updateCloseButtonVisibility(bool active)
		{
			bool DockWidgetClosable = DockWidget->features().testFlag(CDockWidget::DockWidgetClosable);
			bool ActiveTabHasCloseButton = testConfigFlag(CDockManager::ActiveTabHasCloseButton);
			bool AllTabsHaveCloseButton = testConfigFlag(CDockManager::AllTabsHaveCloseButton);
			bool TabHasCloseButton = (ActiveTabHasCloseButton && active) | AllTabsHaveCloseButton;
			CloseButton->setVisible(DockWidgetClosable && TabHasCloseButton);
		}

		/**
		 * @brief Update the size policy of the close button depending on the
		 * RetainTabSizeWhenCloseButtonHidden feature.
		 */
		void updateCloseButtonSizePolicy()
		{
			auto Features = DockWidget->features();
			auto SizePolicy = CloseButton->sizePolicy();
			SizePolicy.setRetainSizeWhenHidden(
				Features.testFlag(CDockWidget::DockWidgetClosable) && testConfigFlag(CDockManager::RetainTabSizeWhenCloseButtonHidden)
			);
			CloseButton->setSizePolicy(SizePolicy);
		}

		template<typename T>
		/**
		 * @brief Creates floating widget.
		 */
		IFloatingWidget* createFloatingWidget(T* Widget, bool CreateContainer)
		{
			if (CreateContainer)
			{
				return new CFloatingDockContainer(Widget);
			}
			else
			{
				auto w = new CFloatingDragPreview(Widget);
				_this->connect(w, &CFloatingDragPreview::draggingCanceled, [this]() {
					DragState = DraggingInactive;
				});
				return w;
			}
		}

		/**
		 * @brief Saves the drag start position in global and local coordinates.
		 */
		void saveDragStartMousePosition(const QPoint& GlobalPos)
		{
			GlobalDragStartMousePosition = GlobalPos;
			DragStartMousePosition = _this->mapFromGlobal(GlobalPos);
		}

		/**
		 * @brief Update the icon in case the icon size changed.
		 */
		void updateIcon()
		{
			if (!IconLabel || Icon.isNull())
			{
				return;
			}

			if (IconSize.isValid())
			{
				IconLabel->setPixmap(Icon.pixmap(IconSize));
			}
			else
			{
				IconLabel->setPixmap(Icon.pixmap(_this->style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, _this)));
			}
			IconLabel->setVisible(true);
		}

		/**
		 * @brief Convenience function for access to the dock manager dock focus controller.
		 */
		CDockFocusController* focusController() const
		{
			return DockWidget->dockManager()->dockFocusController();
		}

		/**
		 * @brief Helper function to create and initialize the menu entries for
		 * the "Auto Hide Group To..." menu.
		 */
		QAction* createAutoHideToAction(const QString& Title, SideBarLocation Location, QMenu* Menu)
		{
			auto Action = Menu->addAction(Title);
			Action->setProperty(internal::LocationProperty, Location);
			QObject::connect(Action, &QAction::triggered, _this, &CDockWidgetTab::onAutoHideToActionClicked);
			return Action;
		}

		/// @brief The this.
		CDockWidgetTab* _this;
		/// @brief The dock widget.
		CDockWidget* DockWidget;
		/// @brief The icon label.
		QLabel* IconLabel = nullptr;
		/// @brief The title label.
		tTabLabel* TitleLabel;
		/// @brief The global drag start mouse position.
		QPoint GlobalDragStartMousePosition;
		/// @brief The drag start mouse position.
		QPoint DragStartMousePosition;
		/// @brief The is active tab.
		bool IsActiveTab = false;
		/// @brief The dock area.
		CDockAreaWidget* DockArea = nullptr;
		/// @brief The drag state.
		eDragState DragState = DraggingInactive;
		/// @brief The floating widget.
		IFloatingWidget* FloatingWidget = nullptr;
		/// @brief The icon.
		QIcon Icon;
		/// @brief The close button.
		QAbstractButton* CloseButton = nullptr;
		/// @brief The icon text spacer.
		QSpacerItem* IconTextSpacer;
		/// @brief The tab drag start position.
		QPoint TabDragStartPosition;
		/// @brief The icon size.
		QSize IconSize;
};
}// namespace ads
