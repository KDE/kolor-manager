/********************************************************************
 KolorServer - color server based on the X Color Management Specification
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

#include <KPluginFactory>
#include <KPluginLoader>

#include "kolor-server.h"

#include "color-lookup-table.h"
#include "display.h"
#include "screen.h"


K_PLUGIN_FACTORY(KolorServerFactory, registerPlugin<KolorServer::Server>();)
K_EXPORT_PLUGIN(KolorServerFactory("kolorserver"))


namespace KolorServer
{

/*
 * Server
 */

Server::Server(QObject *parent, const QList<QVariant> &)
    : KDEDModule(parent)
{
    qDBusRegisterMetaType< Clut >();
    qDBusRegisterMetaType< ClutList >();
    qDBusRegisterMetaType< RegionalClut >();
    qDBusRegisterMetaType< RegionalClutMap >();

    Display::getInstance();

    new ServerDBusAdaptor(this);
}

Server::~Server()
{
    Display::cleanup();
}


/*
 * Server D-Bus Adaptor
 */

ServerDBusAdaptor::ServerDBusAdaptor(Server *server)
    : QDBusAbstractAdaptor(server)
    , m_server(server)
{
    Display *d = Display::getInstance();
    connect(d->screen(), SIGNAL(outputClutsChanged()), this, SIGNAL(outputClutsChanged()));
}

void ServerDBusAdaptor::getVersionInfo(const QDBusMessage &message)
{
    //message.setDelayedReply(true);
    QDBusMessage reply = message.createReply(QVariant((uint) 0x00000001));
    QDBusConnection::sessionBus().send(reply);
}

void ServerDBusAdaptor::getOutputCluts(const QDBusMessage &message)
{
    QDBusMessage reply = message.createReply(
        QVariant::fromValue< ClutList >(Display::getInstance()->screen()->outputCluts()));
    QDBusConnection::sessionBus().send(reply);
}

void ServerDBusAdaptor::getRegionCluts(const QDBusMessage &message)
{
    QDBusMessage reply = message.createReply(
        QVariant::fromValue< RegionalClutMap >(Display::getInstance()->screen()->regionCluts()));
    QDBusConnection::sessionBus().send(reply);
}

} // KolorServer namespace
