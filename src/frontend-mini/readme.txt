This boundle use
	clucene, from Qt source distribution, <qt_sources_root>/src/3rdparty/clucene should be copied to <bibletime_git_root>/../clucene
	sword, it is linked as static library, it is slightly modified for wince platform. path for sources: <bibletime_git_root>/../sword
	qt, is slightly modified for wince platform (raise sip panel and align it to the bottom of screen)
cmake is used to generate projects to build sword library
	for wince use modified cmake version that is able to generate projects for this platform (should be in public access)
	required variables are: -DCMAKE_WINCE_SDK="Windows Mobile 5.0 Pocket PC SDK (ARMV4I)" -DPORT=WinCE
Android
    set QT_USE_ANDROID_NATIVE_STYLE=0 in QtActivity.java for necessitas
    used default modifications for *.pro file with slightly modified QtActivity.java file
	added permission for vibration
	QtCore, QtGui, QtXml and QtSvg libs