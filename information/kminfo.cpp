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

#include <KAboutData>
#include <KGenericFactory>

#include <oyranos.h>
#include <sy_info.h>

// Code to provide KDE module functionality for color manager

K_PLUGIN_FACTORY( kminfoFactory,
         registerPlugin<kminfo>();
         )
K_EXPORT_PLUGIN( kminfoFactory("kminfo") )


kminfo::kminfo(QWidget *parent, const QVariantList &) :
    KCModule( kminfoFactory::componentData(), parent)
{
       KAboutData* about = new KAboutData(
        "kminfo", 0, ki18n("KMInfo"), 0, KLocalizedString(),
        KAboutData::License_BSD,
        ki18n("(c) 2008 Joseph Simon III")
    );
    about->addAuthor( ki18n("2008-2009 Joseph Simon III"), KLocalizedString(),
                     "j.simon.iii@astound.net" );
    about->addAuthor( ki18n("2010-2014 Kai-Uwe Behrmann"), KLocalizedString(),
                      "ku.b@gmx.de"  );

    setAboutData( about );

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

