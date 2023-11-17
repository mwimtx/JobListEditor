################################################################################
#
#

#QMAKE_LFLAGS += -Wl,-rpath,$$PROJECT_LIB_DIR

# DEFINES +=  DEBUG_OUTPUT
DEFINES += VERSION_STRING="0.1.100000.1."
DEFINES += APPLICATION_NAME="JobListEditor"
DEFINES += ORGANISATION_NAME="Metronix"

QMAKE_CXXFLAGS_RELEASE += -Wno-reorder
QMAKE_CXXFLAGS_DEBUG += -Wno-reorder

dwin64 {
QMAKE_CXXFLAGS_RELEASE += -fpermissive
QMAKE_CXXFLAGS_DEBUG += -fpermissive
}
