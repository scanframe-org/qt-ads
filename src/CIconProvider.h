#pragma once

/// @file   CIconProvider.h
/// @author Uwe Kindler
/// @date   18.10.2019
/// @brief  Declaration of CIconProvider.

#include <QIcon>
#include "ads_globals.h"

namespace ads
{

struct IconProviderPrivate;

/**
 * @brief This object provides all icons that are required by the advanced docking
 * system.
 * The IconProvider enables the user to register custom icons in case using
 * stylesheets is not an option.
 */
class ADS_EXPORT CIconProvider
{
	public:
		/**
		 * @brief Default Constructor.
		 */
		CIconProvider();

		/**
		 * @brief Virtual Destructor.
		 */
		virtual ~CIconProvider();

		/**
		 * @brief The function returns a custom icon if one is registered and a null Icon
		 * if no custom icon is registered.
		 */
		QIcon customIcon(eIcon IconId) const;

		/**
		 * @brief Registers a custom icon for the given IconId.
		 */
		void registerCustomIcon(eIcon IconId, const QIcon& icon);

	private:
		friend struct IconProviderPrivate;
		/// Private data (pimpl).
		IconProviderPrivate* d;
};

}// namespace ads
