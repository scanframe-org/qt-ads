
#pragma once

/// @file   CResizeHandle_p.h
/// @author Uwe Kindler
/// @date   24.10.2022
/// @brief  Private data of CResizeHandle class.

#include <QDebug>
#include <QPointer>
#include <QStyleOption>
#include "CResizeHandle.h"
#include "ads_globals.h"

namespace ads
{
/**
 * @brief Private data class of CResizeHandle class (pimpl).
 */
struct ResizeHandlePrivate
{

		/**
		 * @brief Private data constructor.
		 */
		ResizeHandlePrivate(CResizeHandle* _public);

		/**
		 * @brief Pick position component from pos depending on orientation.
		 */
		int pick(const QPoint& pos) const;

		/**
		 * @brief Returns true, if orientation is horizontal.
		 */
		bool isHorizontal() const;

		/**
		 * @brief Set rubberband position.
		 */
		void setRubberBand(int Pos);

		/**
		 * @brief Calculates the resize position and geometry.
		 */
		void doResizing(QMouseEvent* e, bool ForceResize = false);

		/// @brief The this.
		CResizeHandle* _this;
		/// @brief The handle position.
		Qt::Edge HandlePosition = Qt::LeftEdge;
		/// @brief The target.
		QWidget* Target = nullptr;
		/// @brief The mouse offset.
		int MouseOffset = 0;
		/// @brief The pressed.
		bool Pressed = false;
		/// @brief The min size.
		int MinSize = 0;
		/// @brief The max size.
		int MaxSize = 1;
		/// @brief The rubber band.
		QPointer<QRubberBand> RubberBand;
		/// @brief The opaque resize.
		bool OpaqueResize = false;
		/// @brief The handle width.
		int HandleWidth = 4;
};
}// namespace ads
