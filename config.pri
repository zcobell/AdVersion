win32{
    QADCMODULES_PATH = C:/libraries/QADCModules
    LIBGIT2_PATH     = C:/libraries/libgit2
}
unix{
    QADCMODULES_PATH = /opt/QADCModules
    LIBGIT2_PATH     = /usr



#...Special includes for Travis-CI. This may not be 
#   necessary for most users, but in the case it is,
#   modifiy this, otherwise comment it out

    QMAKE_CXXFLAGS   = -I./../thirdparty/libgit2/include
}
