/*
    scriptingplugin.h

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

#ifndef SCRIPTINGPLUGIN_H
#define SCRIPTINGPLUGIN_H

#include <qobject.h>
#include <qvariant.h>

#include <kopete/kopetemessage.h>
#include <kopete/kopeteplugin.h>
#include <kopete/kopetechatsessionmanager.h>
#include <kopete/ui/kopeteview.h>

//#include <kopete/kopeteview.h>

//namespace Kopete { class Message; }
//namespace Kopete { class MetaContact; }
//namespace Kopete { class ChatSession; }

namespace Kross {
    class Action;
}

class ScriptingPlugin : public Kopete::Plugin
{
    	Q_OBJECT
    public:
        static ScriptingPlugin* plugin();
    
        ScriptingPlugin(QObject* parent, const QVariantList& args);
        virtual ~ScriptingPlugin();
        virtual void aboutToUnload();

    public Q_SLOTS:

        void slotMessageReceived(Kopete::Message& msg);
        void slotMessageSent(Kopete::Message& msg);
        void slotItemChanged(const QString& file, bool enabled);
        void slotSettingsChanged();
        
        void slotActionFinished(Kross::Action*);
        void slotActionFinalized(Kross::Action*);

        // this will be called by Kopete::CommandHandler
        void commandExecuted(const QString& command, Kopete::ChatSession* chatsessions);

    private:
        class Private;
        Private* const d;
};

#endif
