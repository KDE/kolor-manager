/********************************************************************
 KolorServer - color server based on the X Color Management Specification
 This file is part of the KDE project.

Copyright (C) 2012 Casian Andrei <skeletk13@gmail.com>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

#include <KPluginFactory>
#include <KPluginLoader>

#include "kolor-server.h"

#include "color-lookup-table.h"
#include "screen.h"

#include "display.h"


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

#include "moc_kolor-server.cpp"
