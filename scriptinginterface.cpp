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

#include <QDBusConnection>

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
        list << (QObject*) c;
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

ScriptingChat::ScriptingChat(QObject *parent, Kopete::ChatSession *chat) : QObject(parent), KXMLGUIClient(chat), m_chat(chat)
{
    setObjectName("KopeteChat");

    m_signalMapper = new QSignalMapper(this);
    connect(m_signalMapper, SIGNAL(mapped(const QString &)), this, SIGNAL(actionTriggered(const QString &)));

    //connect(m_chat, SIGNAL(closing(Kopete::ChatSession*)), SIGNAL(closing()));
    connect(m_chat, SIGNAL(messageAppended(Kopete::Message&)), SLOT(emitAppended(Kopete::Message&)));
    connect(m_chat, SIGNAL(messageReceived(Kopete::Message&)), SLOT(emitReceived(Kopete::Message&)));
    connect(m_chat, SIGNAL(messageSent(Kopete::Message&)), SLOT(emitSent(Kopete::Message&)));

    setXMLFile("scripting.rc");

    unplugActionList("scripting_menu_tools");
    QList<QAction*> contactlist;
    KAction* aaa = new KAction(KIcon("document-export"), "MyOtherTestAction", this);
    actionCollection()->addAction ( "mytestaction", aaa);
    contactlist << aaa;
    plugActionList( "scripting_menu_tools", contactlist );

    KAction *action = new KAction ( KIcon ( "document-encrypt" ), "MyTestAction1" /*i18n("")*/, this );
    actionCollection()->addAction ( "actionSendAdvert", action );
   
}

ScriptingChat::~ScriptingChat() {}
Kopete::ChatSession* ScriptingChat::chat() const { return m_chat; }

QObject* ScriptingChat::addAction(const QString& name, const QString& text, const QString& icon)
{
    KAction *action = new KAction(KIcon(icon), text, this);
    action->setObjectName(name);
    connect(action, SIGNAL(triggered()), m_signalMapper, SLOT(map()));
    m_signalMapper->setMapping(action, name);
    actionCollection()->addAction(name, action);
    return action;
}

QVariantList ScriptingChat::members() const
{
    QVariantList list;
    foreach(Kopete::Contact* c, m_chat->members())
        list << (QObject*) c;
    return list;
}

QObject* ScriptingChat::myself() const { return const_cast<Kopete::Contact*>( m_chat->myself() ); }
QObject* ScriptingChat::account() const { return m_chat->account(); }

const QString ScriptingChat::displayName() { return m_chat->displayName(); }
void ScriptingChat::setDisplayName(const QString& displayname) { m_chat->setDisplayName(displayname); }

void ScriptingChat::append(const QString &subject, const QString& body, bool isHtml)
{
    Kopete::Message msg;
    msg.setSubject(subject);
    if( isHtml )
        msg.setHtmlBody(body);
    else
        msg.setPlainBody(body);
    m_chat->appendMessage(msg);
}

void ScriptingChat::emitAppended(Kopete::Message& msg) {
    ScriptingMessage message(&msg);
    emit appended(&message);
}
void ScriptingChat::emitReceived(Kopete::Message& msg) {
    ScriptingMessage message(&msg);
    emit received(&message);
}
void ScriptingChat::emitSent(Kopete::Message& msg) {
    ScriptingMessage message(&msg);
    emit sent(&message);
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

void ScriptingInterface::emitCommandExecuted(const QString& command, Kopete::ChatSession* chatsessions)
{
    kDebug()<<"ScriptingInterface::emitCommandExecuted command="<<command;
    int idx = d->kChats.indexOf(chatsessions);
    Q_ASSERT( idx >= 0 );
    if( idx < 0 )
        return;
    Q_ASSERT( idx < d->vChats.count() );
    QVariant v = d->vChats[idx];
    QObject* obj = v.value<QObject*>();
    Q_ASSERT( obj );
    ScriptingChat* chat = dynamic_cast<ScriptingChat*>(obj);
    Q_ASSERT( chat );
    Q_ASSERT( chat->chat() == chatsessions );
    if( chat )
        emit commandExecuted(command, chat);
}

QObject* ScriptingInterface::interface()
{
    //FIXME this isn't only ugly and not needed, but also limits us to
    //the dbus-types. So,better solution would be to provide direct
    //access to the KopeteDBusInterface instance and return it here.

    if( ! d->dbusiface )
        d->dbusiface = QDBusConnection::sessionBus().objectRegisteredAt("/Kopete");
    return d->dbusiface;
}

QVariantList ScriptingInterface::chats()
{
    return d->vChats;
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
