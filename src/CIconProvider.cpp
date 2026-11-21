/// @file   CIconProvider.cpp
/// @author Uwe Kindler
/// @date   18.10.2019
/// @brief  Implementation of CIconProvider

#include "CIconProvider.h"
#include "CIconProvider_p.h"

namespace ads
{

IconProviderPrivate::IconProviderPrivate(CIconProvider* _public)
	: _this(_public)
{}

CIconProvider::CIconProvider()
	: d(new IconProviderPrivate(this))
{}

CIconProvider::~CIconProvider()
{
	delete d;
}

QIcon CIconProvider::customIcon(eIcon IconId) const
{
	Q_ASSERT(IconId < d->UserIcons.size());
	return d->UserIcons[IconId];
}

void CIconProvider::registerCustomIcon(eIcon IconId, const QIcon& icon)
{
	Q_ASSERT(IconId < d->UserIcons.size());
	d->UserIcons[IconId] = icon;
}

}// namespace ads
