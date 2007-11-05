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

//#include "kopetemanager.h"
//#include "kopetemessage.h"
//#include "kopeteaccount.h"
//#include "kopetecontact.h"
//#include "kopetechatsessionmanager.h"
//#include "kopetechatsession.h"

#include <QObject>
//#include <kdebug.h>

/// \internal private class for the \a ScriptingInterface class.
class ScriptingInterfacePrivate : public QObject
{
        Q_OBJECT
    public:
        ScriptingInterface* interface;
        QObject* dbusiface;
        QList<Kopete::ChatSession*> kChats; //to be sure we keep a copy of the sessions list
        QVariantList vChats; //QList<ScriptingChatSession*>

        explicit ScriptingInterfacePrivate(ScriptingInterface* iface) : interface(iface), dbusiface(0)
        {
            foreach(Kopete::ChatSession* chatsessions, Kopete::ChatSessionManager::self()->sessions())
                addChat(chatsessions);

            connect(Kopete::ChatSessionManager::self(), SIGNAL(chatSessionCreated(Kopete::ChatSession*)), this, SLOT(addChat(Kopete::ChatSession*)));
            connect(this, SIGNAL(chatAdded(QObject*)), interface, SIGNAL(chatAdded(QObject*)));
            connect(this, SIGNAL(chatRemoved(QObject*)), interface, SIGNAL(chatRemoved(QObject*)));
        }

        virtual ~ScriptingInterfacePrivate() {}

    public Q_SLOTS:

        void addChat(Kopete::ChatSession* chatsessions)
        {
            connect(chatsessions, SIGNAL(closing(Kopete::ChatSession*)), this, SLOT(removeChat(Kopete::ChatSession*)));
            kChats << chatsessions;
            QVariant v;
            ScriptingChat* chat = new ScriptingChat(this, chatsessions);
            v.setValue( (QObject*) chat );
            vChats << v;
            emit chatAdded(chat);
        }

        void removeChat(Kopete::ChatSession* chatsessions)
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
