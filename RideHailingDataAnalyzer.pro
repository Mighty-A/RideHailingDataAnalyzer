TEMPLATE = subdirs

SUBDIRS += src \
    extern/MapGraphics

src.depends += extern/MapGraphics
