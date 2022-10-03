TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = libarchive
SUBDIRS += QArchive

INCLUDEPATH += QArchive \
           QArchive/include
           libarchive/libarchive
