/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 2012 Casian Andrei <skeletk13@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#ifndef KOLOR_SERVER_H_
#define KOLOR_SERVER_H_

#include <KDEDModule>

#include <QDBusAbstractAdaptor>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusMetaType>


namespace KolorServer
{

class Server : public KDEDModule
{
    Q_OBJECT

public:
    Server(QObject *parent, const QList<QVariant> &);
    virtual ~Server();
};

/*
 * D-Bus
 */
class ServerDBusAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.KolorServer")

public:
    explicit ServerDBusAdaptor(Server *server);

public slots:
    Q_NOREPLY void getVersionInfo(const QDBusMessage &message);
    Q_NOREPLY void getOutputCluts(const QDBusMessage &message);
    Q_NOREPLY void getRegionCluts(const QDBusMessage &message);

signals:
    void outputClutsChanged();
    void regionClutsChanged();

private:
    Server *m_server;
};


} // KolorServer namespace

#endif // KOLOR_SERVER_H_
