/*
    scriptinginterface_p.h

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

#ifndef SCRIPTINGINTERFACE_P_H
#define SCRIPTINGINTERFACE_P_H

#include "scriptinginterface.h"
#include "scriptingplugin.h"

#include <QObject>
#include <QSignalMapper>

#include <kopete/kopetepluginmanager.h>
#include <kopete/kopetecontactlist.h>
#include <kopete/kopetemetacontact.h>

/// \internal private class for the \a ScriptingChat class.
class ScriptingChatPrivate : public QObject
{
        Q_OBJECT
    public:
        ScriptingChat *m_chat;
        ScriptingInterface *m_iface;
        Kopete::ChatSession *m_chatsession;
        QSignalMapper *m_signalMapper;
        QList<QAction*> m_actions;

        explicit ScriptingChatPrivate(ScriptingChat *chat) : m_chat(chat)
        {
            m_signalMapper = new QSignalMapper(this);
            connect(m_signalMapper, SIGNAL(mapped(QString)), this, SLOT(actionExecuted(QString)));
        
            //connect(m_chatsessionsession, SIGNAL(closing(Kopete::ChatSession*)), SIGNAL(closing()));
            //connect(m_chatsession, SIGNAL(messageAppended(Kopete::Message&)), SLOT(emitAppended(Kopete::Message&)));
            //connect(m_chatsession, SIGNAL(messageReceived(Kopete::Message&)), SLOT(emitReceived(Kopete::Message&)));
            //connect(m_chatsession, SIGNAL(messageSent(Kopete::Message&)), SLOT(emitSent(Kopete::Message&)));
        
            //connect(Kopete::ChatSessionManager::self(), SIGNAL(viewCreated(KopeteView*)), SLOT(viewCreated(KopeteView*)));
            connect(Kopete::ChatSessionManager::self(), SIGNAL(viewActivated(KopeteView*)), SLOT(viewActivated(KopeteView*)));
            //connect(Kopete::ChatSessionManager::self(), SIGNAL(viewClosing(KopeteView*)), SLOT(viewClosing(KopeteView*)));
        }

    public Q_SLOTS:

        void viewActivated(KopeteView*)
        {
            m_chat->unplugActionList("scripting_chat_tools");
            m_chat->plugActionList("scripting_chat_tools", m_actions);
        }

        void actionExecuted(const QString &name)
        {
            m_iface->emitChatActionExecuted(m_chat, name);
        }

};

/// \internal private class for the \a ScriptingInterface class.
class ScriptingInterfacePrivate : public QObject
{
        Q_OBJECT
    public:
        ScriptingInterface* interface;
        QObject* dbusiface;
        QList<Kopete::ChatSession*> kChats; //to be sure we keep a copy of the sessions list
        QVariantList vChats; //QList<ScriptingChat*>
        QSignalMapper *m_signalMapper;
        QList<QAction*> m_contactActions;

        explicit ScriptingInterfacePrivate(ScriptingInterface *iface) : interface(iface), dbusiface(0)
        {
            m_signalMapper = new QSignalMapper(this);
            connect(m_signalMapper, SIGNAL(mapped(QString)), this, SLOT(contactActionExecuted(QString)));
            connect(Kopete::PluginManager::self(), SIGNAL(allPluginsLoaded()), SLOT(slotAllPluginsLoaded()));
            connect(Kopete::ContactList::self(), SIGNAL(metaContactSelected(bool)), SLOT(slotContactSelected(bool)));

            connect(Kopete::ChatSessionManager::self(), SIGNAL(chatSessionCreated(Kopete::ChatSession*)), this, SLOT(addChat(Kopete::ChatSession*)));
            foreach(Kopete::ChatSession* chatsessions, Kopete::ChatSessionManager::self()->sessions())
                addChat(chatsessions);
            connect(this, SIGNAL(chatAdded(QObject*)), interface, SIGNAL(chatAdded(QObject*)));
            connect(this, SIGNAL(chatRemoved(QObject*)), interface, SIGNAL(chatRemoved(QObject*)));
        }

        virtual ~ScriptingInterfacePrivate() {}

    public Q_SLOTS:

        void slotAllPluginsLoaded()
        {
            ScriptingPlugin::plugin()->unplugActionList("scripting_main_contact");
            ScriptingPlugin::plugin()->plugActionList("scripting_main_contact", m_contactActions);
        }

        void slotContactSelected(bool selected)
        {
            foreach(QAction* a, m_contactActions)
                a->setEnabled(selected);
        }

        void contactActionExecuted(const QString &name)
        {
            Kopete::MetaContact *m = Kopete::ContactList::self()->selectedMetaContacts().first();
            if( ! m )
                return;
            //m->setPluginData( this, "languageKey", m_languages->languageKey( m_actionLanguage->currentItem() ) );
            interface->emitContactActionExecuted(m, name);
        }

        void addChat(Kopete::ChatSession *chatsessions)
        {
            connect(chatsessions, SIGNAL(closing(Kopete::ChatSession*)), this, SLOT(removeChat(Kopete::ChatSession*)));
            kChats << chatsessions;
            QVariant v;
            ScriptingChat* chat = new ScriptingChat(interface, chatsessions);
            v.setValue( (QObject*) chat );
            vChats << v;
            emit chatAdded(chat);
        }

        void removeChat(Kopete::ChatSession *chatsessions)
        {
            int idx = 0;
            while(true) {
                idx = kChats.indexOf(chatsessions, idx);
                if( idx < 0 )
                    break;
                kChats.removeAt(idx);
                QObject* object = vChats.takeAt(idx).value<QObject*>();
                Q_ASSERT(object);
                Q_ASSERT(dynamic_cast<ScriptingChat*>(object));
                emit chatRemoved(object);
                delete object;
            }
        }

    Q_SIGNALS:

        void chatAdded(QObject*);
        void chatRemoved(QObject*);

};

#endif
