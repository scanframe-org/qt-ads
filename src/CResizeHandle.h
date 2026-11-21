#pragma once

/// @file   CResizeHandle.h
/// @author Uwe Kindler
/// @date   24.10.2022
/// @brief  Declaration of CResizeHandle class.

#include <QFrame>

#include "ads_globals.h"

namespace ads
{
struct ResizeHandlePrivate;

/**
 * @brief Resize handle for resizing its parent widget.
 */
class ADS_EXPORT CResizeHandle : public QFrame
{
		Q_OBJECT

	public:
		/**
		 * @brief Default Constructor.
		 */
		CResizeHandle(Qt::Edge HandlePosition, QWidget* parent);

		/**
		 * @brief Virtual Destructor.
		 */
		~CResizeHandle() override;

	private:
		Q_DISABLE_COPY(CResizeHandle)
		Q_PROPERTY(bool opaqueResize READ opaqueResize WRITE setOpaqueResize)

	protected:
		/**
		 * @brief Handles the mouse move event.
		 */
		void mouseMoveEvent(QMouseEvent*) override;
		/**
		 * @brief Handles the mouse press event.
		 */
		void mousePressEvent(QMouseEvent*) override;
		/**
		 * @brief Handles the mouse release event.
		 */
		void mouseReleaseEvent(QMouseEvent*) override;

	public:
		/**
		 * @brief Alias for the base class.
		 */
		using Super = QFrame;

		/**
		 * @brief Sets the handle position.
		 */
		void setHandlePosition(Qt::Edge HandlePosition);

		/**
		 * @brief Returns the handle position.
		 */
		Qt::Edge handlePostion() const;

		/**
		 * @brief Returns the orientation of this resize handle.
		 */
		Qt::Orientation orientation() const;

		/**
		 * @brief Returns the size hint.
		 */
		QSize sizeHint() const override;

		/**
		 * @brief Returns true, if resizing is active.
		 */
		bool isResizing() const;

		/**
		 * @brief Sets the minimum size for the widget that is going to be resized.
		 * The resize handle will not resize the target widget to a size smaller
		 * than this value.
		 */
		void setMinResizeSize(int MinSize);

		/**
		 * @brief Sets the maximum size for the widget that is going to be resized
		 * The resize handle will not resize the target widget to a size bigger
		 * than this value.
		 */
		void setMaxResizeSize(int MaxSize);

		/**
		 * @brief Enable / disable opaque resizing.
		 */
		void setOpaqueResize(bool opaque = true);

		/**
		 * @brief Returns true if widgets are resized dynamically (opaquely) while
		 * interactively moving the resize handle. Otherwise returns false.
		 */
		bool opaqueResize() const;

	private:
		friend struct ResizeHandlePrivate;
		/// Private data (pimpl).
		ResizeHandlePrivate* d;
};
}// namespace ads
