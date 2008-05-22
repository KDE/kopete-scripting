/*
    scriptingpreferences.cpp

    Copyright (c) 2007      by Sebastian Sauer <mail@dipe.org>

    ***************************************************************************
    *                                                                         *
    *   This program is free software; you can redistribute it and/or modify  *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    *                                                                         *
    ***************************************************************************
*/

#include "scriptingpreferences.h"

#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QTimer>

#include <kgenericfactory.h>
#include <kfiledialog.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <kdebug.h>

#include <kross/core/manager.h>
#include <kross/core/interpreter.h>

#include <kopete/kopeteplugin.h>
#include <kopete/kopetepluginmanager.h>

K_PLUGIN_FACTORY(ScriptingPreferencesFactory, registerPlugin<ScriptingPreferences>();)
K_EXPORT_PLUGIN(ScriptingPreferencesFactory( "kcm_kopete_scripting" ))

class ScriptingPreferences::Private
{
    public:
        bool isInitialized;
        QListWidget* list;

        explicit Private() : isInitialized(false) {}

        QListWidgetItem* addItem(const QString& file, bool checked = false) {
            const QString interpreter = Kross::Manager::self().interpreternameForFile(file);
            QListWidgetItem* item = new QListWidgetItem(KIcon(interpreter), file, list);
            if( interpreter.isEmpty() || ! QFileInfo(file).exists() ) {
                item->setFlags( Qt::ItemIsSelectable );
                checked = false;
            }
            item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
            return item;
        }
};

ScriptingPreferences::ScriptingPreferences( QWidget *parent, const QVariantList &args )
    : KCModule(ScriptingPreferencesFactory::componentData(), parent, args)
    , d(new Private())
{
    QVBoxLayout* layout = new QVBoxLayout;
    setLayout( layout );
    layout->setSpacing(6);
    layout->setMargin(9);

    d->list = new QListWidget(this);
    connect(d->list, SIGNAL(itemChanged(QListWidgetItem*)), SLOT(slotItemChanged(QListWidgetItem*)));
    layout->addWidget(d->list);

    QHBoxLayout* btnLayout = new QHBoxLayout;
    layout->addLayout(btnLayout);

    QPushButton *addbtn = new QPushButton(i18n("Add..."), this);
    connect(addbtn, SIGNAL(clicked()), SLOT(slotAddBtnClicked()));
    btnLayout->addWidget(addbtn);

    QPushButton *rembtn = new QPushButton(i18n("Remove"), this);
    connect(rembtn, SIGNAL(clicked()), SLOT(slotRemBtnClicked()));
    btnLayout->addWidget(rembtn);

    //resize( QSize(520, 340).expandedTo( minimumSizeHint() ) );
    setMinimumSize( QSize(520, 340) );

    Kopete::Plugin* plugin = Kopete::PluginManager::self()->plugin("kopete_scripting");
    if( plugin ) // plugin is not loaded yet cause its deactivated.
        connect(this, SIGNAL(itemChanged(QString,bool)), plugin, SLOT(slotItemChanged(QString,bool)));

    QTimer::singleShot(0, this, SLOT(load()));
}

ScriptingPreferences::~ScriptingPreferences()
{
    delete d;
}

void ScriptingPreferences::slotItemChanged(QListWidgetItem* item)
{
    if( d->isInitialized && item ) {
        emit itemChanged(item->text(), item->checkState() == Qt::Checked);
        emit KCModule::changed(true);
    }
}

void ScriptingPreferences::slotAddBtnClicked()
{
    QStringList mimetypes;
    foreach(QString interpreter, Kross::Manager::self().interpreters())
        if( Kross::InterpreterInfo* info = Kross::Manager::self().interpreterInfo(interpreter) )
            mimetypes << info->mimeTypes().join(" ").trimmed();
    KFileDialog dlg(
        KUrl("kfiledialog:///KopeteAddScript"), // startdir
        mimetypes.join(" "), // filter
        0, // custom widget
        0 ); // parent
    dlg.setCaption(i18n("Add Script File..."));
    dlg.setOperationMode(KFileDialog::Opening);
    dlg.setMode(KFile::File | KFile::ExistingOnly | KFile::LocalOnly);
    if( dlg.exec() ) {
        d->list->blockSignals(true);
        QListWidgetItem* item = d->addItem( dlg.selectedUrl().path() );
        d->list->setCurrentItem(item);
        d->list->blockSignals(false);
        emit KCModule::changed(true);
    }
}

void ScriptingPreferences::slotRemBtnClicked()
{
    QListWidgetItem* item = d->list->currentItem();
    if( ! item )
        return;
    emit itemChanged(item->text(), item->checkState() == Qt::Checked);
    delete item;
    emit KCModule::changed(true);
}

void ScriptingPreferences::save()
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group(config, "scripts");
    QStringList files_enabled, files_disabled;
    for(int i = 0; i < d->list->count(); ++i)
        if( d->list->item(i)->checkState() == Qt::Checked )
            files_enabled << d->list->item(i)->text();
        else
            files_disabled << d->list->item(i)->text();
    group.writeEntry("files_enabled", files_enabled);
    group.writeEntry("files_disabled", files_disabled);
    config->sync();
    emit KCModule::changed(false);
}

void ScriptingPreferences::load()
{
    d->list->blockSignals(true);
    d->list->clear();
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group(config, "scripts");
    QStringList files_enabled = group.readEntry("files_enabled", QStringList());
    foreach(QString file, files_enabled)
        d->addItem( file, true );
    QStringList files_disabled = group.readEntry("files_disabled", QStringList());
    if( ! d->isInitialized ) {
        KStandardDirs dirs;
        foreach(QString interpreter, Kross::Manager::self().interpreters())
            if( Kross::InterpreterInfo* info = Kross::Manager::self().interpreterInfo(interpreter) )
                foreach(QString filter, info->wildcard().split(" "))
                    foreach(QString file, dirs.findAllResources("data", QString("kopete/scripts/%1").arg(filter)))
                        if( ! files_disabled.contains(file) && ! files_enabled.contains(file) )
                            files_disabled << file;
        d->isInitialized = true;
    }
    foreach(QString file, files_disabled)
        d->addItem( file );
    d->list->blockSignals(false);
    emit KCModule::changed(false);
}

void ScriptingPreferences::defaults()
{
    d->isInitialized = false;
    load();
}

#include "scriptingpreferences.moc"
