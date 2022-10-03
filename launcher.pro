TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = libs \
          launcher

INCLUDEPATH += . libs/QArchive \
               libs/QArchive/include
               libs/libarchive/libarchive
