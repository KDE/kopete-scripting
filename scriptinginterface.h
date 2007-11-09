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
#include <kopete/ui/kopeteview.h>

#include <QObject>
#include <QPointer>
#include <kxmlguiclient.h>
#include <kactioncollection.h>
#include <kdebug.h>

class ScriptingPlugin;
class ScriptingInterface;
class ScriptingInterfacePrivate;
class ScriptingChatPrivate;

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

        /// Accessor method for the \a Kopete::Contact objects that sent this message.
        QObject* sender() const;
        /// Accessor method for the list of \a Kopete::Contact objects that this message was sent to.
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
        ScriptingChat(ScriptingInterface *iface, Kopete::ChatSession *chatsession);
        virtual ~ScriptingChat();
        Kopete::ChatSession* chat() const;

    public Q_SLOTS:

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
        void appendMessage(const QString& body, const QString &subject = QString(), bool isHtml = true /*, const QString &from = QString(), const QStringList &to = QStringList()*/);
        /// Send a message to the chat session.
        void sendMessage(const QString& body, const QString &subject = QString(), bool isHtml = true);

        /// Add a new action to the chat window.
        QObject* addAction(const QString& name, const QString& text, const QString& icon = QString());

    Q_SIGNALS:

        /// This signal is emitted if a new message in the chat got appended.
        //void appended(ScriptingMessage* message);
        /// This signal is emitted if a new message in the chat got received.
        //void received(ScriptingMessage* message);
        /// This signal is emitted if a new message in the chat got sent.
        //void sent(ScriptingMessage* message);

    private:
        ScriptingChatPrivate* const d;
};

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
        void emitCommandExecuted(Kopete::ChatSession* chatsessions, const QString& command, const QStringList& args);
        void emitChatActionExecuted(ScriptingChat* chat, const QString &name) { emit chatActionExecuted(chat, name); }
        void emitContactActionExecuted(QObject* contact, const QString &name) { emit contactActionExecuted(contact, name); }
        void emitSettingsChanged() { emit settingsChanged(); }

    public Q_SLOTS:

        /// Return the \a KopeteDBusInterface instance.
        QObject* interface();

        //QVariantList protocols();

        /// Return a list of \a ScriptingChat instances.
        QVariantList chats();
        //QObject* createChat( const Kopete::Contact *user, Kopete::ContactPtrList chatContacts, Kopete::Protocol *protocol);
        //void removeSession( Kopete::ChatSession *session );
        //QObject* findChat( const Kopete::Contact *user, Kopete::ContactPtrList chatContacts, Kopete::Protocol *protocol);

        /// Return a list of \a Kopete::Account instances.
        QVariantList accounts();
        /// Returns a list of all \a Kopete::MetaContact instances of the contact list.
        QVariantList contacts();

        //QVariantList groups();

        /// Add a new action to the main window.
        QObject* addContactAction(const QString& name, const QString& text, const QString& icon = QString());

        /// Returns true if there exist a command with the identifier \p command .
        bool hasCommand(const QString &command);
        /// Add a new command. If the command got executed the commandExecuted() signal will be emitted.
        void addCommand(const QString &command, const QString &help = QString(), int minArgs = 0, int maxArgs = -1, const QString &icon = QString());
        /// Remove an existing command.
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

        /// This signal got emitted if a custom command got executed.
        void commandExecuted(QObject* chat, const QString& command, const QStringList& args);
        /// This signal got emitted if a custom chat-action got executed.
        void chatActionExecuted(QObject* chat, const QString &name);
        /// This signal got emitted if a custom contact-action got executed.
        void contactActionExecuted(QObject* contact, const QString &name);

        /// This signal got emitted if the Kopete settings changed.
        void settingsChanged();

    private:
        ScriptingInterfacePrivate* const d;
};

#endif
