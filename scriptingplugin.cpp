/*
    scriptingplugin.cpp

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

#include "scriptingplugin.h"
#include "scriptinginterface.h"

#include <QApplication>
#include <QCursor>
#include <kgenericfactory.h>
#include <kmessagebox.h>
#include <kactioncollection.h>

#include <kross/core/manager.h>
#include <kross/core/action.h>
#include <kross/core/actioncollection.h>

K_PLUGIN_FACTORY(ScriptingPluginFactory, registerPlugin<ScriptingPlugin>();)
K_EXPORT_PLUGIN(ScriptingPluginFactory("kopete_scripting", "kcm_kopete_scripting"))

class ScriptingPlugin::Private
{
    public:
        static ScriptingPlugin* pluginStatic;
        ScriptingInterface* interface;
        Kross::ActionCollection* collection;
        
        Kross::Action* createAction(const QString& file) {
            Kross::Action* action = new Kross::Action(pluginStatic, KUrl(file));
            action->addObject(interface, "Kopete", Kross::ChildrenInterface::AutoConnectSignals);
            collection->addAction(action);
            QObject::connect(action, SIGNAL(finished(Kross::Action*)), pluginStatic, SLOT(slotActionFinished(Kross::Action*)));
            QObject::connect(action, SIGNAL(finalized(Kross::Action*)), pluginStatic, SLOT(slotActionFinalized(Kross::Action*)));
            return action;
        }

        void triggerAction(Kross::Action* action) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            action->trigger();
            QApplication::restoreOverrideCursor();
            interface->emitPluginInit();
        }
};

ScriptingPlugin* ScriptingPlugin::Private::pluginStatic = 0L;

ScriptingPlugin::ScriptingPlugin(QObject* parent, const QVariantList&)
    : Kopete::Plugin(ScriptingPluginFactory::componentData(), parent), d(new Private())
{
	if( ! Private::pluginStatic )
        Private::pluginStatic = this;

    setXMLFile("scriptingui.rc");

    d->interface = new ScriptingInterface(this);
    d->collection = new Kross::ActionCollection("plugins", Kross::Manager::self().actionCollection());

	connect(Kopete::ChatSessionManager::self(), SIGNAL(aboutToReceive(Kopete::Message&)), SLOT(slotMessageReceived(Kopete::Message&)));
    connect(Kopete::ChatSessionManager::self(), SIGNAL(aboutToSend(Kopete::Message&)), SLOT(slotMessageSent(Kopete::Message&)));   
    connect(this, SIGNAL(settingsChanged()), SLOT(slotSettingsChanged()));

    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group(config, "scripts");
    foreach(QString file, group.readEntry("files_enabled", QStringList()))
        d->triggerAction( d->createAction(file) );
}

ScriptingPlugin::~ScriptingPlugin()
{
    Private::pluginStatic = 0L;
    delete d->collection;
    delete d;   
}

ScriptingPlugin* ScriptingPlugin::plugin()
{
	return Private::pluginStatic;
}

void ScriptingPlugin::aboutToUnload()
{
    kDebug()<<"ScriptingPlugin::aboutToUnload";
    d->interface->emitPluginFinish();
    Kopete::Plugin::aboutToUnload();
}

void ScriptingPlugin::slotMessageReceived( Kopete::Message& msg )
{
    kDebug()<<"ScriptingPlugin::slotReceivedMessage subject="<<msg.subject()<<"plainBody="<<msg.plainBody();
    ScriptingMessage message(&msg);
    d->interface->emitMessageReceived(&message);
}

void ScriptingPlugin::slotMessageSent(Kopete::Message& msg)
{
    kDebug()<<"ScriptingPlugin::slotMessageSent subject="<<msg.subject()<<"plainBody="<<msg.plainBody();
    ScriptingMessage message(&msg);
    d->interface->emitMessageSent(&message);
}

void ScriptingPlugin::slotItemChanged(const QString& file, bool enabled)
{
    kDebug()<<"ScriptingPlugin::slotItemChanged file="<<file<<"enabled="<<enabled;

    Kross::Action* action = d->collection->action(file);
    if( action ) {
        if( ! enabled ) {
            d->collection->removeAction(action);
            delete action;
            return;
        }
    }
    else {
        if( ! enabled )
            return;
        action = d->createAction(file);
    }

    if( enabled )
        d->triggerAction(action);
}

void ScriptingPlugin::slotActionFinished(Kross::Action*)
{
    kDebug()<<"ScriptingPlugin::slotActionFinished";
}

void ScriptingPlugin::slotActionFinalized(Kross::Action*)
{
    kDebug()<<"ScriptingPlugin::slotActionFinalized";
    /*
    if( action->hadError() ) {
        if( action->errorTrace().isNull() )
            KMessageBox::error(0, action->errorMessage());
        else
            KMessageBox::detailedError(0, action->errorMessage(), action->errorTrace());
    }
    */
}

void ScriptingPlugin::commandExecuted(const QString& command, Kopete::ChatSession* chatsessions)
{
    Q_ASSERT( QObject::sender() );
    if( ! QObject::sender() )
        return;
    QString name = QObject::sender()->objectName();
    if( name.toLower().endsWith("_command") )
        name = name.left( name.count() - 8 );
    kDebug()<<"ScriptingPlugin::commandExecuted name="<<name<<"command="<<command;
    QStringList args = Kopete::CommandHandler::parseArguments(command);
    d->interface->emitCommandExecuted(chatsessions, name, args);
}

void ScriptingPlugin::slotSettingsChanged()
{
    d->interface->viewUpdated();
}

#include "scriptingplugin.moc"
