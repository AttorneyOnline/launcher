TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = libs \
          launcher

INCLUDEPATH += libs/QArchive \
               libs/QArchive/include \
               libs/libarchive/libarchive

win32:LIBS += -LC:/Program Files (x86)/libarchive/lib
