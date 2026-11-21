#pragma once

/// @file   CFloatingDragPreview.h
/// @author Uwe Kindler
/// @date   26.11.2019
/// @brief  Declaration of CFloatingDragPreview.

#include "CFloatingDockContainer.h"

namespace ads
{
class CDockWidget;
class CDockAreaWidget;
class CDockContainerWidget;
struct FloatingDragPreviewPrivate;

/**
 * @brief A floating overlay is a temporary floating widget that is just used to
 * indicate the floating widget movement.
 * This widget is used as a placeholder for drag operations for non-opaque
 * docking.
 */
class CFloatingDragPreview
	: public QWidget
	, public IFloatingWidget
{
		Q_OBJECT

	protected:
		/**
		 * @brief The content is a DockArea or a DockWidget.
		 */
		CFloatingDragPreview(QWidget* Content, QWidget* parent);

	public:
		/**
		 * @brief Creates an instance for undocking the DockWidget in Content parameter.
		 */
		CFloatingDragPreview(CDockWidget* Content);

		/**
		 * @brief Creates an instance for undocking the DockArea given in Content
		 * parameters.
		 */
		CFloatingDragPreview(CDockAreaWidget* Content);

		/**
		 * @brief Delete private data.
		 */
		~CFloatingDragPreview();

	private Q_SLOTS:
		/**
		 * @brief Cancel non opaque undocking if application becomes inactive.
		 */
		void onApplicationStateChanged(Qt::ApplicationState state);

	protected:
		/**
		 * @brief Cares about painting the.
		 */
		void paintEvent(QPaintEvent* e) override;

	public:
		/**
		 * @brief Alias for the base class.
		 */
		using Super = QWidget;

		/**
		 * @brief We filter the events of the assigned content widget to receive
		 * escape key presses for canceling the drag operation.
		 */
		bool eventFilter(QObject* watched, QEvent* event) override;

		// implements IFloatingWidget -----------------------------------------
		/**
		 * @brief Starts the floating operation.
		 */
		void startFloating(const QPoint& DragStartMousePos, const QSize& Size, eDragState DragState, QWidget* MouseEventHandler) override;

		/**
		 * @brief Moves the widget to a new position relative to the position given when
		 * startFloating() was called.
		 */
		void moveFloating() override;

		/**
		 * @brief Wayland hybrid drag: move the preview using an explicit global position
		 * supplied by the grabbing tab/title-bar mouse event instead of
		 * QCursor::pos(), which is unreliable on Wayland. Also drives the drop
		 * overlays from the supplied position.
		 */
		void moveFloating(const QPoint& GlobalPos);

		/**
		 * @brief Wayland hybrid drag: confine drop-target hit-testing and overlays to a
		 * single source container during the in-window phase, so nothing leaks to
		 * other top-level windows and no cross-window global coordinate is needed.
		 */
		void setSourceContainer(CDockContainerWidget* Container);

		/**
		 * @brief Wayland hybrid drag: tear the preview down without performing a drop and
		 * without emitting draggingCanceled. Used when the in-window drag converts
		 * to a native platform drag on a window-boundary cross.
		 */
		void cancelDraggingSilently();

		/**
		 * @brief Finishes dragging.
		 * Hides the dock overlays and executes the real undocking and docking
		 * of the assigned Content widget.
		 */
		void finishDragging() override;

		/**
		 * @brief Cleanup auto hide container if the dragged widget has one.
		 */
		void cleanupAutoHideContainerWidget(DockWidgetArea ContainerDropArea);

	Q_SIGNALS:
		/**
		 * @brief This signal is emitted, if dragging has been canceled by escape key
		 * or by active application switching via task manager.
		 */
		void draggingCanceled();

	private:
		friend struct FloatingDragPreviewPrivate;
		/// @brief The d.
		FloatingDragPreviewPrivate* d;
};

}// namespace ads
