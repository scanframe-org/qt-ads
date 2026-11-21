
#pragma once

/// @file   CIconProvider_p.h
/// @author Uwe Kindler
/// @date   18.10.2019
/// @brief  Private data of CIconProvider.

#include "CIconProvider.h"

#include <QVector>

namespace ads
{
/**
 * @brief Private data class (pimpl).
 */
struct IconProviderPrivate
{

		/**
		 * @brief Private data constructor.
		 */
		IconProviderPrivate(CIconProvider* _public);
		/// @brief The this.
		CIconProvider* _this;
		/// @brief The user icons.
		QVector<QIcon> UserIcons{IconCount, QIcon()};
};
}// namespace ads
