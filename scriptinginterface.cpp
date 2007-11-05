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

#include <QDBusConnection>

ScriptingInterface::ScriptingInterface(QObject *parent)
    : QObject(parent)
    , d(new ScriptingInterfacePrivate(this))
{
    setObjectName("Kopete");
}

ScriptingInterface::~ScriptingInterface()
{
    delete d;
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

#include "scriptinginterface_p.moc"
#include "scriptinginterface.moc"
