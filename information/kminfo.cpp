/*

Copyright (C) 2008 Joseph Simon III <j.simon.iii@astound.net>

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

*/


#include "kminfo.h"

#include <kcmodule.h>
#include <kaboutdata.h>
#include <KLocalizedString>
#include <kpluginfactory.h>

#include <oyranos.h>
#include <sy_info.h>

// Code to provide KDE module functionality for color manager

K_PLUGIN_FACTORY( kminfoFactory,
         registerPlugin<kminfo>();
         )


kminfo::kminfo(QWidget *parent, const QVariantList &args ) :
    KCModule( parent, args )
{
    const char    * name = NULL;
    oyWidgetTitleGet( oyWIDGET_GROUP_INFORMATION, NULL, &name,
                      NULL, NULL );
    QString title = QString("KolorManager Oyranos %1").arg(name);
    KAboutData * about = new KAboutData("kminfo",
		    title, "1.1.0",
                    i18n("KDE version of Oyranos Synnefo"),
                    KAboutLicense::BSDL,
                    i18n("(c) 2008 Joseph Simon III"),
		    "",
		    "http://www.oyranos.org/kolormanager");

    about->addAuthor(i18n("Joseph Simon III"), i18n("developer"), "j.simon.iii@astound.net", "https://jsimon3.wordpress.com/");
    about->addAuthor(i18n("Kai-Uwe Behrmann"), i18n("developer"), "ku.b@gmx.de", "http://www.behrmann.name/");
    about->addAuthor(i18n("Jan Gruhlich"), i18n("developer"), "jgrulich@redhat.com", 0);

    about->addAuthor(i18n("Albert Astals Cid"), i18n("developer"), "tsdgeos@terra.es", 0);
    about->addAuthor(i18n("Christoph Feck"), i18n("developer"), "christoph@maxiom.de", 0);
    about->addAuthor(i18n("Boudewijn Rempt "), i18n("developer"), "boud@valdyas.org", "http://www.valdyas.org/fading/index.cgi");
    about->addAuthor(i18n("Pino Toscano"), i18n("developer"), "pino@kde.org", 0);
    about->addAuthor(i18n("Laurent Montel"), i18n("developer"), "montel@kde.org", 0);
    about->addAuthor(i18n("Cyrille Berger Skott"), i18n("developer"), "cberger@cberger.net", 0);
    about->addAuthor(i18n("Hal Van Engel"), i18n("developer"), "hvengel@gmail.com", 0);

    setAboutData(about);

    oyMessageFuncSet( oyGuiMessageFunc );

    module = new SyInfoModule(0);       // "Synnefo Info"

    QGridLayout *gridLayout;
    gridLayout = new QGridLayout(this);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    gridLayout->addWidget(module, 0, 0, 0, 0);
}


kminfo::~kminfo()
{
    delete module;
}

#include "kminfo.moc"
