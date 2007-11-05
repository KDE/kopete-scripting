/*
    scriptingpreferences.h

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

#ifndef SCRIPTINGPREFERENCES_H
#define SCRIPTINGPREFERENCES_H

#include <kcmodule.h>

class QListWidgetItem;

class ScriptingPreferences : public KCModule
{
    	Q_OBJECT
    public:
        ScriptingPreferences( QWidget *parent = 0, const QVariantList &args = QVariantList() );
        virtual ~ScriptingPreferences();

    public Q_SLOTS:
        void save();
        void load();
        void defaults();

    Q_SIGNALS:
        void itemChanged(const QString& file, bool enabled);

    private Q_SLOTS:
        void slotAddBtnClicked();
        void slotRemBtnClicked();
        void slotItemChanged(QListWidgetItem* item);

    private:
        class Private;
        Private* const d;
};

#endif
