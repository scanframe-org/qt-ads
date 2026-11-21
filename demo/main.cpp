#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QString>
#include <QThreadPool>

#include <MainWindow.h>

#include <memory>

// MinGW GCC < 13.4 detection for thread_local destructor ordering bug GCC
// #116159"
#if (defined(__MINGW32__) || defined(__MINGW64__)) && ((__GNUC__ < 13) || (__GNUC__ == 13 && __GNUC_MINOR__ < 4))
	#define IS_MINGW_THREADLOCAL_BUGGY 1
#else
	#define IS_MINGW_THREADLOCAL_BUGGY 0
#endif

void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	QByteArray localMsg = msg.toLocal8Bit();
	switch (type)
	{
		case QtDebugMsg:
			fprintf(stdout, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
			break;
		case QtInfoMsg:
			fprintf(stdout, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
			break;
		case QtWarningMsg:
			fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
			break;
		case QtCriticalMsg:
			fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
			break;
		case QtFatalMsg:
			fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
			abort();
	}

	fflush(stderr);
	fflush(stdout);
}

int main(int argc, char* argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
	#if QT_VERSION >= 0x050600
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	#endif
#endif
	QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
	std::shared_ptr<int> b;
	QApplication a(argc, argv);
	a.setApplicationName("Advanced Docking System Demo");
	a.setQuitOnLastWindowClosed(true);
	a.setWindowIcon(QIcon(":/adsdemo/images/ads_icon2.svg"));

	qInstallMessageHandler(myMessageOutput);
	qDebug() << "Message handler test";

	CMainWindow mw;
	mw.show();

	QFile StyleSheetFile(":/adsdemo/app.css");
	if (StyleSheetFile.open(QIODevice::ReadOnly))
	{
		QTextStream StyleSheetStream(&StyleSheetFile);
		a.setStyleSheet(StyleSheetStream.readAll());
		StyleSheetFile.close();
	}
	// Fix for hanging Qt threads in Wine since 6.9.1
	const auto exit_code = QApplication::exec();
#if IS_MINGW_THREADLOCAL_BUGGY
	QThreadPool::globalInstance()->waitForDone();
	exit(exit_code);
#endif
	return exit_code;
}
