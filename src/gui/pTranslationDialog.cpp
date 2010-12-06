#include "pTranslationDialog.h"
#include "ui_pTranslationDialog.h"
#include "core/pTranslationManager.h"
#include "pIconManager.h"
#include "listeditor/pPathListEditor.h"

#include <QFileDialog>
#include <QDebug>

pTranslationDialog::pTranslationDialog( pTranslationManager* translationManager, QWidget* parent )
	: QDialog( parent )
{
	Q_ASSERT( translationManager );
	mTranslationManager = translationManager;
	
	ui = new Ui::pTranslationDialog;
	ui->setupUi( this );
	ui->tbReload->click();
}

pTranslationDialog::~pTranslationDialog()
{
	delete ui;
}

QString pTranslationDialog::selectedLocale() const
{
	const QTreeWidgetItem* currentItem = ui->twLocales->selectedItems().value( 0 );
	return currentItem ? currentItem->data( 0, Qt::UserRole ).toString() : mTranslationManager->currentLocale().name();
}

QString pTranslationDialog::getLocale( pTranslationManager* translationManager, QWidget* parent )
{
	pTranslationDialog dlg( translationManager, parent );
	
	if ( dlg.exec() == QDialog::Accepted ) {
		return dlg.selectedLocale();
	}
	
	return QString::null;
}

void pTranslationDialog::on_tbLocate_clicked()
{
	QDialog dlg( this );
	pPathListEditor editor( tr( "Choose folders containing your application translations" ), QApplication::applicationDirPath(), &dlg );
	QDialogButtonBox buttons( &dlg );
	QVBoxLayout vl( &dlg );
	vl.addWidget( &editor );
	vl.addWidget( &buttons );
	
	buttons.setStandardButtons( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
	editor.setValues( mTranslationManager->translationsPaths() );
	
	connect( &buttons, SIGNAL( rejected() ), &dlg, SLOT( reject() ) );
	connect( &buttons, SIGNAL( accepted() ), &dlg, SLOT( accept() ) );
	
	if ( dlg.exec() == QDialog::Rejected ) {
		return;
	}
	
	mTranslationManager->setTranslationsPaths( editor.values() );
	ui->tbReload->click();
}

QTreeWidgetItem* pTranslationDialog::newItem( const QLocale& locale )
{
	const QString language = QLocale::languageToString( locale.language() );
	const QString country = QLocale::countryToString( locale.country() );
	const QString countryCode = locale.name().section( '_', 1 );
	
	QTreeWidgetItem* item = new QTreeWidgetItem;
	item->setIcon( 0, pIconManager::icon( QString( "%1.png" ).arg( countryCode.toLower() ), ":/fresh/country-flags" ) );
	item->setText( 0, QString( "%1 (%2)" ).arg( language ).arg( country ) );
	item->setToolTip( 0, locale.name() );
	item->setData( 0, Qt::UserRole, locale.name() );
	return item;
}

QTreeWidgetItem* pTranslationDialog::rootItem( const QLocale& locale )
{
	QTreeWidgetItem* item = mRootItems.value( locale.name() );
	
	if ( !item ) {
		item = newItem( locale );
		ui->twLocales->addTopLevelItem( item );
		mRootItems[ locale.name() ] = item;
	}
	
	return item;
}

void pTranslationDialog::on_tbReload_clicked()
{
	// reload translations if needed
	if ( mTranslationManager->availableLocales().isEmpty() ) {
		mTranslationManager->reloadTranslations();
	}
	
	// keep current locale
	const QString currentLocale = selectedLocale();
	
	// clear items
	ui->twLocales->clear();
	mRootItems.clear();
	
	// create new ones
	foreach ( const QLocale& _locale, mTranslationManager->availableQLocales() ) {
		const QLocale locale = _locale.language() == QLocale::C ? QLocale( QLocale::English ) : _locale;
		QTreeWidgetItem* rootItem = this->rootItem( QLocale( locale.language() ) );
		
		if ( rootItem->data( 0, Qt::UserRole ).toString() == locale.name() ) {
			continue;
		}
		
		rootItem->addChild( newItem( locale ) );
	}
	
	// sort items
	ui->twLocales->sortByColumn( 0, Qt::AscendingOrder );
	
	// restore locale
	QAbstractItemModel* model = ui->twLocales->model();
	QModelIndex index = model->match( model->index( 0, 0 ), Qt::UserRole, currentLocale, 1, Qt::MatchFixedString | Qt::MatchWrap ).value( 0 );
	
	if ( !index.isValid() ) {
		index = model->match( model->index( 0, 0 ), Qt::UserRole, currentLocale, 1, Qt::MatchStartsWith | Qt::MatchWrap ).value( 0 );
	}
	
	ui->twLocales->setCurrentIndex( index );
}
