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

#include <QObject>
#include <QPointer>
#include <kdebug.h>

class ScriptingMessage : public QObject
{
        Q_OBJECT
    public:
        explicit ScriptingMessage(Kopete::Message *message) : QObject(), m_message(message) { setObjectName("KopeteMessage"); }
        virtual ~ScriptingMessage() {}
        Kopete::Message* message() const { return m_message; }

    public Q_SLOTS:

        /// Accessor method for the Contact that sent this message.
        QObject* sender() const { return const_cast<Kopete::Contact*>( m_message->from() ); }
        /// Accessor method for the Contacts that this message was sent to.
        QVariantList receiver() const {
            QVariantList list;
            foreach(Kopete::Contact* c, m_message->to())
                list << (QObject*) c;
            return list;
        }

        /// Get the message timestamp.
        QDateTime timestamp() const { return m_message->timestamp(); }
        /// Set the message timestamp.
        void setTimestamp(const QDateTime &timestamp) { m_message->setTimestamp(timestamp); }

        /// Return the subject.
        QString subject() const { return m_message->subject(); }
        /// Set the subject.
        void setSubject(const QString &subject) { m_message->setSubject(subject); }

        /// Get the message body back as plain text.
        QString plainBody() const { return m_message->plainBody(); }
        /// Get the message body as escaped (X)HTML format.
        QString escapedBody() const { return m_message->escapedBody(); }
        /// Get the message body as parsed HTML with Emoticons, and URL parsed.
        QString parsedBody() const { return m_message->parsedBody(); }
        /// Sets the body of the message as plain text.
        void setPlainBody(const QString &body) { m_message->setPlainBody(body); }
        /// Sets the body of the message as escaped (X)HTML format.
        void setHtmlBody(const QString &body) { m_message->setHtmlBody(body); }

        /// Return the foreground color.
        QColor foregroundColor() const { return m_message->foregroundColor(); }
        /// Set the foreground color.
        void setForegroundColor(const QColor& color) { m_message->setForegroundColor(color); }
        /// Return the background color.
        QColor backgroundColor() const { return m_message->backgroundColor(); }
        /// Set the background color.
        void setBackgroundColor(const QColor& color) { m_message->setBackgroundColor(color); }

    private:
        Kopete::Message* m_message;
};

class ScriptingChat : public QObject
{
        Q_OBJECT
    public:
        ScriptingChat(QObject *parent, Kopete::ChatSession *chat) : QObject(parent), m_chat(chat) {
            setObjectName("KopeteChat");
            //connect(m_chat, SIGNAL(closing(Kopete::ChatSession*)), SIGNAL(closing()));
            connect(m_chat, SIGNAL(messageAppended(Kopete::Message&)), SLOT(emitAppended(Kopete::Message&)));
            connect(m_chat, SIGNAL(messageReceived(Kopete::Message&)), SLOT(emitReceived(Kopete::Message&)));
            connect(m_chat, SIGNAL(messageSent(Kopete::Message&)), SLOT(emitSent(Kopete::Message&)));
        }
        virtual ~ScriptingChat() {}
        Kopete::ChatSession* chat() const { return m_chat; }

    public Q_SLOTS:

        /// Get a list of \a Kopete::Contact instance of all contacts in the chat session.
        QVariantList members() const {
            QVariantList list;
            foreach(Kopete::Contact* c, m_chat->members())
                list << (QObject*) c;
            return list;
        }

        /// Return a \a Kopete::Contact object.
        QObject* myself() const { return const_cast<Kopete::Contact*>( m_chat->myself() ); }
        /// Return a \a Kopete::Account object.
        QObject* account() const { return m_chat->account(); }

        /// Return the display-caption of the chat.
        const QString displayName() { return m_chat->displayName(); }
        /// Set the display-caption of the chat.
        void setDisplayName(const QString& displayname) { m_chat->setDisplayName(displayname); }

        /// Append a message to the chat session.
        void append(const QString &subject, const QString& body, bool isHtml = true /*, const QString &from = QString(), const QStringList &to = QStringList()*/) {
            Kopete::Message msg;
            msg.setSubject(subject);
            if( isHtml )
                msg.setHtmlBody(body);
            else
                msg.setPlainBody(body);
            m_chat->appendMessage(msg);
        }

        //void appendMessage(ScriptingMessage* msg) {}
        //void sendMessage(ScriptingMessage* msg);

    Q_SIGNALS:
        //void closing();
        void appended(ScriptingMessage* message);
        void received(ScriptingMessage* message);
        void sent(ScriptingMessage* message);

    private Q_SLOTS:
        void emitAppended(Kopete::Message& msg) {
            ScriptingMessage message(&msg);
            emit appended(&message);
        }
        void emitReceived(Kopete::Message& msg) {
            ScriptingMessage message(&msg);
            emit received(&message);
        }
        void emitSent(Kopete::Message& msg) {
            ScriptingMessage message(&msg);
            emit sent(&message);
        }

    private:
        Kopete::ChatSession *m_chat;
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
        explicit ScriptingInterface( QObject *parent );
        virtual ~ScriptingInterface();

        void emitPluginInit() { emit pluginInit(); }
        void emitPluginFinish() { emit pluginFinish(); }
        void emitMessageReceived(ScriptingMessage* message) { emit messageReceived(message); }
        void emitMessageSent(ScriptingMessage* message) { emit messageSent(message); }
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

        //void viewOpened(QObject* view);
        //void viewActivated(QObject* view);
        //void viewClosed(QObject* view);

        /// This signal got emitted if the Kopete settings changed.
        void settingsChanged();

    private:
        ScriptingInterfacePrivate* const d;
};

#endif
