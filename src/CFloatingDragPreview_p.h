
#pragma once

/// @file   CFloatingDragPreview_p.h
/// @author Uwe Kindler
/// @date   26.11.2019
/// @brief  Private data of CFloatingDragPreview.

#include "CFloatingDragPreview.h"

#include <QApplication>
#include <QEvent>
#include <QPainter>
#include "CDockAreaWidget.h"
#include "CDockContainerWidget.h"
#include "CDockManager.h"
#include "CDockOverlay.h"
#include "CDockWidget.h"
#include "ads_globals.h"

namespace ads
{

/**
 * @brief Private data class (pimpl).
 */
struct FloatingDragPreviewPrivate
{

		/**
		 * @brief Private data constructor.
		 */
		FloatingDragPreviewPrivate(CFloatingDragPreview* _public);

		/**
		 * @brief Returns the last event-supplied global position when available (Wayland
		 * hybrid drag), otherwise the live cursor position.
		 */
		QPoint cursorPos() const
		{
			return HasLastGlobalPos ? LastGlobalPos : QCursor::pos();
		}

		/**
		 * @brief Updates drop overlays.
		 */
		void updateDropOverlays(const QPoint& GlobalPos);

		/**
		 * @brief Sets the hidden.
		 */
		void setHidden(bool Value)
		{
			Hidden = Value;
			_this->update();
		}

		/**
		 * @brief Cancel dragging and emit the draggingCanceled event.
		 */
		void cancelDragging()
		{
			Canceled = true;
			Q_EMIT _this->draggingCanceled();
			DockManager->containerOverlay()->hideOverlay();
			DockManager->dockAreaOverlay()->hideOverlay();
			_this->close();
		}

		/**
		 * @brief Creates the real floating widget in case the mouse is released outside
		 * outside of any drop area.
		 */
		void createFloatingWidget();

		/**
		 * @brief Returns true, if the content is floatable.
		 */
		bool isContentFloatable() const
		{
			return this->ContentFeatures.testFlag(CDockWidget::DockWidgetFloatable);
		}

		/**
		 * @brief Returns true, if the content is pinnable.
		 */
		bool isContentPinnable() const
		{
			return this->ContentFeatures.testFlag(CDockWidget::DockWidgetPinnable);
		}

		/**
		 * @brief Returns the content features.
		 */
		CDockWidget::DockWidgetFeatures contentFeatures() const
		{
			CDockWidget* DockWidget = qobject_cast<CDockWidget*>(Content);
			if (DockWidget)
			{
				return DockWidget->features();
			}

			CDockAreaWidget* DockArea = qobject_cast<CDockAreaWidget*>(Content);
			if (DockArea)
			{
				return DockArea->features();
			}

			return CDockWidget::DockWidgetFeatures();
		}

		/// @brief The this.
		CFloatingDragPreview* _this;
		/// @brief The content.
		QWidget* Content;
		/// @brief The content features.
		CDockWidget::DockWidgetFeatures ContentFeatures;
		/// @brief The content source area.
		CDockAreaWidget* ContentSourceArea = nullptr;
		/// @brief The drag start mouse position.
		QPoint DragStartMousePosition;
		/// @brief The dock manager.
		CDockManager* DockManager;
		/// @brief The drop container.
		CDockContainerWidget* DropContainer = nullptr;
		/// @brief The window opacity.
		qreal WindowOpacity;
		/// @brief The hidden.
		bool Hidden = false;
		/// @brief The content preview pixmap.
		QPixmap ContentPreviewPixmap;
		/// @brief The canceled.
		bool Canceled = false;

		// Wayland hybrid drag: during the in-window phase, hit-testing and overlays
		// are confined to this container and driven by an explicit, event-supplied
		// global position instead of the unreliable QCursor::pos().
		/// @brief The source container.
		CDockContainerWidget* SourceContainer = nullptr;
		/// @brief The last global pos.
		QPoint LastGlobalPos;
		/// @brief The has last global pos.
		bool HasLastGlobalPos = false;
};
}// namespace ads
