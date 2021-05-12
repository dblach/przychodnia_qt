QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 openssl-linked

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    administratorzy.cpp \
    edycja_administratora.cpp \
    edycja_aktualnosci.cpp \
    edycja_lekarza.cpp \
    edycja_pacjenta.cpp \
    edycja_poradni.cpp \
    edycja_terminu.cpp \
    edycja_wizyty.cpp \
    kalendarz.cpp \
    lekarze.cpp \
    main.cpp \
    mainwindow.cpp \
    moi_pacjenci.cpp \
    moje_wizyty.cpp \
    pacjenci.cpp \
    poradnie.cpp \
    tworzenie_wizyty.cpp \
    uploader.cpp \
    ustawienia.cpp \
    ustawienia_aplikacji.cpp

HEADERS += \
    administratorzy.h \
    edycja_administratora.h \
    edycja_aktualnosci.h \
    edycja_lekarza.h \
    edycja_pacjenta.h \
    edycja_poradni.h \
    edycja_terminu.h \
    edycja_wizyty.h \
    globals.h \
    kalendarz.h \
    lekarze.h \
    mainwindow.h \
    moi_pacjenci.h \
    moje_wizyty.h \
    pacjenci.h \
    poradnie.h \
    strings.h \
    tworzenie_wizyty.h \
    uploader.h \
    ustawienia.h \
    ustawienia_aplikacji.h

FORMS += \
    administratorzy.ui \
    edycja_administratora.ui \
    edycja_aktualnosci.ui \
    edycja_lekarza.ui \
    edycja_pacjenta.ui \
    edycja_poradni.ui \
    edycja_terminu.ui \
    edycja_wizyty.ui \
    kalendarz.ui \
    lekarze.ui \
    mainwindow.ui \
    moi_pacjenci.ui \
    moje_wizyty.ui \
    pacjenci.ui \
    poradnie.ui \
    tworzenie_wizyty.ui \
    uploader.ui \
    ustawienia.ui \
    ustawienia_aplikacji.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
