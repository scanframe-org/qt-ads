/// @file   CDockComponentsFactory.cpp
/// @author Uwe Kindler
/// @date   10.02.2020
/// @brief  Implementation of DockComponentsFactory

#include "CDockComponentsFactory.h"

#include <CAutoHideTab.h>

#include <memory>

#include "CDockAreaTabBar.h"
#include "CDockAreaTitleBar.h"
#include "CDockAreaWidget.h"
#include "CDockWidget.h"
#include "CDockWidgetTab.h"

namespace ads
{

static QSharedPointer<ads::CDockComponentsFactory> DefaultFactory;

CDockWidgetTab* CDockComponentsFactory::createDockWidgetTab(CDockWidget* DockWidget) const
{
	return new CDockWidgetTab(DockWidget);
}

CAutoHideTab* CDockComponentsFactory::createDockWidgetSideTab(CDockWidget* DockWidget) const
{
	return new CAutoHideTab(DockWidget);
}

CDockAreaTabBar* CDockComponentsFactory::createDockAreaTabBar(CDockAreaWidget* DockArea) const
{
	return new CDockAreaTabBar(DockArea);
}

CDockAreaTitleBar* CDockComponentsFactory::createDockAreaTitleBar(CDockAreaWidget* DockArea) const
{
	return new CDockAreaTitleBar(DockArea);
}

QSharedPointer<ads::CDockComponentsFactory> CDockComponentsFactory::factory()
{
	if (!DefaultFactory)
	{
		DefaultFactory.reset(new CDockComponentsFactory());
	}
	return DefaultFactory;
}

void CDockComponentsFactory::setFactory(CDockComponentsFactory* Factory)
{
	DefaultFactory.reset(Factory);
}

void CDockComponentsFactory::resetDefaultFactory()
{
	DefaultFactory.reset(new CDockComponentsFactory());
}

}// namespace ads

// EOF CDockComponentsFactory.cpp
