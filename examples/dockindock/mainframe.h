#pragma once

#include <QAction>
#include <QMainWindow>
#include <QSettings>

#include <memory>

namespace QtAdsUtl
{
class DockInDockWidget;
class PerspectivesManager;
}// namespace QtAdsUtl

class MainWindow : public QMainWindow
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget* parent = 0);
		~MainWindow();

		QtAdsUtl::DockInDockWidget* m_dockManager;
		std::unique_ptr<QtAdsUtl::PerspectivesManager> m_perspectivesManager;
};
