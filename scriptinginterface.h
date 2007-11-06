/*
    scriptinginterface.h

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

#ifndef SCRIPTINGINTERFACE_H
#define SCRIPTINGINTERFACE_H

#include <kopete/kopetemessage.h>
#include <kopete/kopeteaccount.h>
#include <kopete/kopetecontact.h>
#include <kopete/kopetechatsessionmanager.h>
#include <kopete/kopetechatsession.h>
#include <kopete/kopetecommandhandler.h>

#include <QObject>
#include <QPointer>
#include <QSignalMapper>
#include <kxmlguiclient.h>
#include <kactioncollection.h>
#include <kdebug.h>

class ScriptingPlugin;

/**
* The ScriptingMessage class wraps a \a Kopete::Message to provide a scripting API
* to deal with messages within Kopete.
*/
class ScriptingMessage : public QObject
{
        Q_OBJECT
    public:
        explicit ScriptingMessage(Kopete::Message *message);
        virtual ~ScriptingMessage();
        Kopete::Message* message() const;

    public Q_SLOTS:

        /// Accessor method for the Contact that sent this message.
        QObject* sender() const;
        /// Accessor method for the Contacts that this message was sent to.
        QVariantList receiver() const;

        /// Get the message timestamp.
        QDateTime timestamp() const;
        /// Set the message timestamp.
        void setTimestamp(const QDateTime &timestamp);

        /// Return the subject.
        QString subject() const;
        /// Set the subject.
        void setSubject(const QString &subject);

        /// Get the message body back as plain text.
        QString plainBody() const;
        /// Get the message body as escaped (X)HTML format.
        QString escapedBody() const;
        /// Get the message body as parsed HTML with Emoticons, and URL parsed.
        QString parsedBody() const;
        /// Set the body of the message as plain text.
        void setPlainBody(const QString &body);
        /// Set the body of the message as escaped (X)HTML format.
        void setHtmlBody(const QString &body);

        /// Return the foreground color.
        QColor foregroundColor() const;
        /// Set the foreground color.
        void setForegroundColor(const QColor& color);
        /// Return the background color.
        QColor backgroundColor() const;
        /// Set the background color.
        void setBackgroundColor(const QColor& color);

    private:
        Kopete::Message* m_message;
};

/**
* The ScriptingChat class wraps a \a Kopete::ChatSession to provide a scripting API
* to deal with chat-sessions within Kopete.
*/
class ScriptingChat : public QObject, public KXMLGUIClient
{
        Q_OBJECT
    public:
        ScriptingChat(QObject *parent, Kopete::ChatSession *chat);
        virtual ~ScriptingChat();
        Kopete::ChatSession* chat() const;

    public Q_SLOTS:

        /// Add a new action to the chat window.
        QObject* addAction(const QString& name, const QString& text, const QString& icon);

        /// Get a list of \a Kopete::Contact instance of all contacts in the chat session.
        QVariantList members() const;

        /// Return a \a Kopete::Contact object.
        QObject* myself() const;
        /// Return a \a Kopete::Account object.
        QObject* account() const;

        /// Return the display-caption of the chat.
        const QString displayName();
        /// Set the display-caption of the chat.
        void setDisplayName(const QString& displayname);

        /// Append a message to the chat session.
        void append(const QString &subject, const QString& body, bool isHtml = true /*, const QString &from = QString(), const QStringList &to = QStringList()*/);

        //void appendMessage(ScriptingMessage* msg) {}
        //void sendMessage(ScriptingMessage* msg);

    Q_SIGNALS:
        //void closing();

        /// This signal is emitted if a new message in the chat got appended.
        void appended(ScriptingMessage* message);
        /// This signal is emitted if a new message in the chat got received.
        void received(ScriptingMessage* message);
        /// This signal is emitted if a new message in the chat got sent.
        void sent(ScriptingMessage* message);

        /// This signal is emitted if an action added with addAction() got executed.
        void actionTriggered(const QString &name);

    private Q_SLOTS:
        void emitAppended(Kopete::Message& msg);
        void emitReceived(Kopete::Message& msg);
        void emitSent(Kopete::Message& msg);

    private:
        Kopete::ChatSession *m_chat;
        QSignalMapper* m_signalMapper;
};

class ScriptingInterfacePrivate;

/**
* The ScriptingInterface class is the interface for scripts. Each signal the
* class has will be connected with a scripting function the script defines.
* Within the \a ScriptingPlugin class a \a ScriptingInterface instance got
* created and published to the scripting world using Kross.
*/
class ScriptingInterface : public QObject
{
    	Q_OBJECT
    public:
        explicit ScriptingInterface(ScriptingPlugin *plugin);
        virtual ~ScriptingInterface();
        void emitPluginInit() { emit pluginInit(); }
        void emitPluginFinish() { emit pluginFinish(); }
        void emitMessageReceived(ScriptingMessage* message) { emit messageReceived(message); }
        void emitMessageSent(ScriptingMessage* message) { emit messageSent(message); }
        void emitSettingsChanged() { emit settingsChanged(); }
        void emitCommandExecuted(const QString& command, const QStringList& args, Kopete::ChatSession* chatsessions);

    public Q_SLOTS:

        /// Return the \a KopeteDBusInterface instance.
        QObject* interface();

        //QVariantList protocols();

        /// Return a list of \a ScriptingChat instances.
        QVariantList chats();
        //QObject* createChat( const Kopete::Contact *user, Kopete::ContactPtrList chatContacts, Kopete::Protocol *protocol);
        //void removeSession( Kopete::ChatSession *session );
        //QObject* findChat( const Kopete::Contact *user, Kopete::ContactPtrList chatContacts, Kopete::Protocol *protocol);

        bool hasCommand(const QString &command);
        void addCommand(const QString &command, const QString &help = QString(), int minArgs = 0, int maxArgs = -1, const QString &icon = QString());
        void removeCommand(const QString &command);

    Q_SIGNALS:

        /// This signal got emitted if the scripting plugin got loaded.
        void pluginInit();
        /// This signal got emitted if the scripting plugin got unloaded.
        void pluginFinish();

        /// This signal got emitted if a new message received.
        void messageReceived(QObject* message);
        /// This signal got emitted if a new message got sent.
        void messageSent(QObject* message);

        /// This signal got emitted if a new chat got added.
        void chatAdded(QObject* chat);
        /// This signal got emitted if a chat got removed.
        void chatRemoved(QObject* chat);

        /// This signal got emitted if a registered command got executed.
        void commandExecuted(const QString& command, const QStringList& args, QObject* chat);

        /// This signal got emitted if the Kopete settings changed.
        void settingsChanged();

    private:
        ScriptingInterfacePrivate* const d;
};

#endif
