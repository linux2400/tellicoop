/** Simple program to merge several tellico databases,
and generate an HTML front-end */

#include "../document.h"
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include "controller.h"
#include "tellico_kernel.h"
#include "images/imagefactory.h" // needed so tmp files can get cleaned
#include "collections/collectioninitializer.h"
#include "translators/htmlexporter.h"

int main(int argc, char** argv)
{
	using namespace Tellico;
	using Tellico::Data::Document;
	
	KAboutData aboutData("tellico", 0, ki18n("Tellicoop"),
                       "0.1", ki18n("Tellicoop"),
                       KAboutData::License_GPL_V2,
                       ki18n("(c) 2015, Romain Nguyen van yen"), KLocalizedString(),
                       "http://tellico-project.org", "tellico-users@kde.org");

        KCmdLineOptions options;
	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(options);
	KCmdLineArgs* args = KCmdLineArgs::parsedArgs();

	KApplication app;

  //Controller::init(this); // the only time this is ever called!
  // has to be after controller init
  //Kernel::init(this); // the only time this is ever called!
        CollectionInitializer init;
	ImageFactory::init();
	Document* d = Document::self();
	d->openDocument(KUrl("file:///home/rnguyen/ownCloud/linux2400.tc"));

	Export::HTMLExporter* htmlExp = new Export::HTMLExporter(d->collection());
	htmlExp->setGroupBy(QStringList());
	htmlExp->setSortTitles(QStringList() << "Titre" << "Auteur" << "ISBN");
	htmlExp->setColumns(QStringList() << "Auteur" << "Titre" << "Lu" << "Note");
	htmlExp->setURL(KUrl("file:///home/rnguyen/ownCloud/bibliotheque.html"));
	htmlExp->setEntries(d->collection()->entries());
	htmlExp->setEntryXSLTFile("/usr/share/kde4/apps/tellico/entry-templates/Fancy.xsl");
	htmlExp->setExportEntryFiles(true);
	htmlExp->setOptions( Export::ExportFormatted | Export::ExportUTF8 | Export::ExportImages | Export::ExportForce | Export::ExportComplete);
	htmlExp->exec();

}
