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
#include <QAction>
#include <QSignalMapper>

#include <kopete/kopetepluginmanager.h>
#include <kopete/kopetecontactlist.h>
#include <kopete/kopetemetacontact.h>

//#include <kopete/ui/kopeteview.h>
//#include <kopete/ui/kopeteviewplugin.h>

/// \internal private class for the \a ScriptingChat class.
class ScriptingChatPrivate : public QObject
{
        Q_OBJECT
    public:
        ScriptingInterface *m_iface;
        ScriptingChat *m_chat;
        Kopete::ChatSession *m_chatsession;
        QSignalMapper *m_signalMapper;
        QList<QAction*> m_actions;

        explicit ScriptingChatPrivate(ScriptingChat *chat) : QObject(), m_chat(chat)
        {
            m_signalMapper = new QSignalMapper(this);
            connect(m_signalMapper, SIGNAL(mapped(QString)), this, SLOT(actionExecuted(QString)));
        
            //connect(m_chatsessionsession, SIGNAL(closing(Kopete::ChatSession*)), SIGNAL(closing()));
            //connect(m_chatsession, SIGNAL(messageAppended(Kopete::Message&)), SLOT(emitAppended(Kopete::Message&)));
            //connect(m_chatsession, SIGNAL(messageReceived(Kopete::Message&)), SLOT(emitReceived(Kopete::Message&)));
            //connect(m_chatsession, SIGNAL(messageSent(Kopete::Message&)), SLOT(emitSent(Kopete::Message&)));
        
            //connect(Kopete::ChatSessionManager::self(), SIGNAL(viewCreated(KopeteView*)), SLOT(viewCreated(KopeteView*)));
            connect(Kopete::ChatSessionManager::self(), SIGNAL(viewActivated(KopeteView*)), SLOT(viewUpdated()));
            //connect(Kopete::ChatSessionManager::self(), SIGNAL(viewClosing(KopeteView*)), SLOT(viewClosing(KopeteView*)));
        }

    public Q_SLOTS:

        void viewUpdated()
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
        ScriptingInterface* m_interface;
        QObject* m_dbusiface;
        QList<Kopete::ChatSession*> m_kChats; //to be sure we keep a copy of the sessions list
        QVariantList m_vChats; //QList<ScriptingChat*>
        QSignalMapper *m_signalMapper;
        QList<QAction*> m_contactActions;

        explicit ScriptingInterfacePrivate(ScriptingInterface *iface) : QObject(), m_interface(iface), m_dbusiface(0)
        {
            m_signalMapper = new QSignalMapper(this);
            connect(m_signalMapper, SIGNAL(mapped(QString)), this, SLOT(contactActionExecuted(QString)));
            //connect(Kopete::PluginManager::self(), SIGNAL(allPluginsLoaded()), SLOT(viewUpdated()));
            connect(Kopete::PluginManager::self(), SIGNAL(pluginLoaded(Kopete::Plugin*)), SLOT(viewUpdated()));
            connect(Kopete::ContactList::self(), SIGNAL(metaContactSelected(bool)), SLOT(slotContactSelected(bool)));
            connect(Kopete::ChatSessionManager::self(), SIGNAL(chatSessionCreated(Kopete::ChatSession*)), SLOT(slotAddChat(Kopete::ChatSession*)));
        }

        virtual ~ScriptingInterfacePrivate() {}

    public Q_SLOTS:

        void viewUpdated()
        {
            if( m_kChats.count() == 0 )
                foreach(Kopete::ChatSession* chatsessions, Kopete::ChatSessionManager::self()->sessions())
                    slotAddChat(chatsessions);
            ScriptingPlugin::plugin()->unplugActionList("scripting_main_contact");
            ScriptingPlugin::plugin()->plugActionList("scripting_main_contact", m_contactActions);
            foreach(QVariant v, m_vChats) {
                ScriptingChat *chat = dynamic_cast<ScriptingChat*>( v.value<QObject*>() );
                Q_ASSERT(chat);
                chat->viewUpdated();
            }
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
            m_interface->emitContactActionExecuted(m, name);
        }

        void slotAddChat(Kopete::ChatSession *chatsessions)
        {
            connect(chatsessions, SIGNAL(closing(Kopete::ChatSession*)), this, SLOT(slotRemoveChat(Kopete::ChatSession*)));
            m_kChats << chatsessions;
            QVariant v;
            ScriptingChat* chat = new ScriptingChat(m_interface, chatsessions);
            v.setValue( (QObject*) chat );
            m_vChats << v;
            m_interface->emitChatAdded(chat);
        }

        void slotRemoveChat(Kopete::ChatSession *chatsessions)
        {
            int idx = 0;
            while(true) {
                idx = m_kChats.indexOf(chatsessions, idx);
                if( idx < 0 )
                    break;
                m_kChats.removeAt(idx);
                QObject* object = m_vChats.takeAt(idx).value<QObject*>();
                Q_ASSERT(object);
                Q_ASSERT(dynamic_cast<ScriptingChat*>(object));
                m_interface->emitChatRemoved(object);
                delete object;
            }
        }
};

#endif
