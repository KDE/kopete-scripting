/*
    scriptinginterface.cpp

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

#include "scriptinginterface.h"
#include "scriptinginterface_p.h"
#include "scriptingplugin.h"

#include <kopete/kopeteaccount.h>
#include <kopete/kopeteaccountmanager.h>

#include <kplugininfo.h>
//#include <QDBusConnection>
#include <QApplication>
#include <KAction>
#include <KIcon>
/***********************************************************************************************
* ScriptingMessage
*/

ScriptingMessage::ScriptingMessage(Kopete::Message *message) : QObject(), m_message(message)
{
    setObjectName("KopeteMessage");
}

ScriptingMessage::~ScriptingMessage()
{
}

Kopete::Message* ScriptingMessage::message() const
{
    return m_message;
}

QObject* ScriptingMessage::sender() const
{
    return const_cast<Kopete::Contact*>( m_message->from() );
}

QVariantList ScriptingMessage::receiver() const
{
    QVariantList list;
    foreach(Kopete::Contact* c, m_message->to())
        list << qVariantFromValue((QObject*)c);
    return list;
}

QDateTime ScriptingMessage::timestamp() const { return m_message->timestamp(); }
void ScriptingMessage::setTimestamp(const QDateTime &timestamp) { m_message->setTimestamp(timestamp); }

QString ScriptingMessage::subject() const { return m_message->subject(); }
void ScriptingMessage::setSubject(const QString &subject) { m_message->setSubject(subject); }

QString ScriptingMessage::plainBody() const { return m_message->plainBody(); }
QString ScriptingMessage::escapedBody() const { return m_message->escapedBody(); }
QString ScriptingMessage::parsedBody() const { return m_message->parsedBody(); }
void ScriptingMessage::setPlainBody(const QString &body) { m_message->setPlainBody(body); }
void ScriptingMessage::setHtmlBody(const QString &body) { m_message->setHtmlBody(body); }

QColor ScriptingMessage::foregroundColor() const { return m_message->foregroundColor(); }
void ScriptingMessage::setForegroundColor(const QColor& color) { m_message->setForegroundColor(color); }
QColor ScriptingMessage::backgroundColor() const { return m_message->backgroundColor(); }
void ScriptingMessage::setBackgroundColor(const QColor& color) { m_message->setBackgroundColor(color); }

/***********************************************************************************************
* ScriptingChat
*/

ScriptingChat::ScriptingChat(ScriptingInterface *iface, Kopete::ChatSession *chatsession)
    : QObject(iface), KXMLGUIClient(chatsession), d(new ScriptingChatPrivate(this))
{
    d->m_iface = iface;
    d->m_chatsession = chatsession;
    setObjectName("KopeteChat");
    setComponentData(ScriptingPlugin::plugin()->componentData());
    setXMLFile("scriptingchatui.rc");
}

ScriptingChat::~ScriptingChat()
{
    delete d;
}

Kopete::ChatSession* ScriptingChat::chat() const
{
    return d->m_chatsession;
}

void ScriptingChat::viewUpdated()
{
    d->viewUpdated();
}

QObject* ScriptingChat::addAction(const QString& name, const QString& text, const QString& icon)
{
    KAction *action = icon.isEmpty() ? new KAction(text, this) : new KAction(KIcon(icon), text, this);
    action->setObjectName(name);
    connect(action, SIGNAL(triggered()), d->m_signalMapper, SLOT(map()));
    d->m_signalMapper->setMapping(action, name);
    d->m_actions << action;
    return action;
}

QVariantList ScriptingChat::members() const
{
    QVariantList list;
    foreach(Kopete::Contact* c, d->m_chatsession->members()) {
        QVariant v;
        v.setValue( (QObject*) c );
        list << v;
    }
    return list;
}

QObject* ScriptingChat::myself() const
{
    return const_cast<Kopete::Contact*>( d->m_chatsession->myself() );
}

QObject* ScriptingChat::account() const
{
    return d->m_chatsession->account();
}

const QString ScriptingChat::displayName()
{
    return d->m_chatsession->displayName();
}

void ScriptingChat::setDisplayName(const QString& displayname)
{
    d->m_chatsession->setDisplayName(displayname);
}

void ScriptingChat::appendMessage(const QString& body, const QString &subject, bool isHtml)
{
    Kopete::Message msg;
    if( ! subject.isEmpty() )
        msg.setSubject(subject);
    if( isHtml )
        msg.setHtmlBody(body);
    else
        msg.setPlainBody(body);
    d->m_chatsession->appendMessage(msg);
}

void ScriptingChat::sendMessage(const QString& body, const QString &subject, bool isHtml)
{
    const Kopete::Contact *fromContact = d->m_chatsession->myself();
    Q_ASSERT(fromContact);
    const QList<Kopete::Contact*> toContacts = d->m_chatsession->members();
    Q_ASSERT(toContacts.count() > 0); //required!
    Kopete::Message msg(fromContact, toContacts);
    if( ! subject.isEmpty() )
        msg.setSubject(subject);
    if( isHtml )
        msg.setHtmlBody(body);
    else
        msg.setPlainBody(body);
    d->m_chatsession->sendMessage(msg);
}

/***********************************************************************************************
* ScriptingInterface
*/

ScriptingInterface::ScriptingInterface(ScriptingPlugin *plugin)
    : QObject(plugin)
    , d(new ScriptingInterfacePrivate(this))
{
    setObjectName("Kopete");
}

ScriptingInterface::~ScriptingInterface()
{
    delete d;
}

void ScriptingInterface::viewUpdated()
{
    d->viewUpdated();
}

void ScriptingInterface::emitCommandExecuted(Kopete::ChatSession* chatsessions, const QString& command, const QStringList& args)
{
    int idx = d->m_kChats.indexOf(chatsessions);
    Q_ASSERT( idx >= 0 );
    if( idx < 0 )
        return;
    Q_ASSERT( idx < d->m_vChats.count() );
    QVariant v = d->m_vChats[idx];
    QObject* obj = v.value<QObject*>();
    Q_ASSERT( obj );
    ScriptingChat* chat = dynamic_cast<ScriptingChat*>(obj);
    Q_ASSERT( chat );
    Q_ASSERT( chat->chat() == chatsessions );
    if( chat )
        emit commandExecuted(chat, command, args);
}

QObject* ScriptingInterface::interface()
{
    if( ! d->m_dbusiface ) {
        //d->m_dbusiface = QDBusConnection::sessionBus().objectRegisteredAt("/Kopete");
        d->m_dbusiface = qApp->findChild< QObject* >("KopeteDBusInterface");
    }
    return d->m_dbusiface;
}

QVariantList ScriptingInterface::chats()
{
    return d->m_vChats;
}

QVariantList ScriptingInterface::accounts()
{
    QVariantList list;
    foreach(Kopete::Account *a, Kopete::AccountManager::self()->accounts()) {
        QVariant v;
        v.setValue( (QObject*) a );
        list << v;
    }
    return list;
}

QVariantList ScriptingInterface::contacts()
{
    QVariantList list;
    foreach(Kopete::MetaContact *c, Kopete::ContactList::self()->metaContacts()) {
		QVariant v;
		v.setValue( (QObject*) c );
        list << v;
	}
    return list;
}

QObject* ScriptingInterface::addContactAction(const QString& name, const QString& text, const QString& icon)
{
    KAction *action = icon.isEmpty() ? new KAction(text, this) : new KAction(KIcon(icon), text, this);
    action->setObjectName(name);
    connect(action, SIGNAL(triggered()), d->m_signalMapper, SLOT(map()));
    d->m_signalMapper->setMapping(action, name);
    d->m_contactActions << action;
    return action;
}

bool ScriptingInterface::hasCommand(const QString &command)
{
    return Kopete::CommandHandler::commandHandler()->commandHandled(command);
}

void ScriptingInterface::addCommand(const QString &command, const QString &help, int minArgs, int maxArgs, const QString &icon)
{
    Kopete::CommandHandler::commandHandler()->registerCommand(parent(), command, SLOT(commandExecuted(const QString&, Kopete::ChatSession*)), help, minArgs, maxArgs, KShortcut(), icon);
}

void ScriptingInterface::removeCommand(const QString &command)
{
    Kopete::CommandHandler::commandHandler()->unregisterCommand(d, command);
}

#include "scriptinginterface_p.moc"
#include "scriptinginterface.moc"
