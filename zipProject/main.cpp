#include <QtCore/QCoreApplication>
#include <memory.h>
#include <QString.h>
#include "FileZipper.h"
using namespace std;
int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QString currentDirPath = QCoreApplication::applicationDirPath();
	std::vector<std::string> files;
	files.push_back(std::string(currentDirPath.toStdString() + "/file\\1.txt"));
	files.push_back(std::string(currentDirPath.toStdString() + "/file\\2.txt"));
	files.push_back(std::string(currentDirPath.toStdString() + "/file\\3.txt"));
	files.push_back(std::string(currentDirPath.toStdString() + "/file\\DEFAULT/4.txt"));
	QString savePath = currentDirPath+"//123.zip";
	std::string zipfileName(std::string(savePath.toStdString()));
	FileZipper aZipper;
	int res = aZipper.ZipFiles(zipfileName, files);
	return a.exec();
}
