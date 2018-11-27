TEMPLATE = subdirs

SUBDIRS += \
    tests \
    libs

tests.depends = libs
