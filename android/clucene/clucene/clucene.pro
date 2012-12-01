#-------------------------------------------------
#
# Project created by QtCreator 2012-10-20T23:25:29
#
#-------------------------------------------------

QT += gui

TARGET = clucene
TEMPLATE = lib
CONFIG += staticlib

DEFINES += _UCS2 _CL_DISABLE_MULTITHREADING

#symbian {
#DEFINES += __GNUC__
#}

INCLUDEPATH += . \
    ../../../../clucene/src \

SOURCES += \
    ../../../../clucene/src/CLucene/analysis/AnalysisHeader.cpp \
    ../../../../clucene/src/CLucene/analysis/Analyzers.cpp \
    ../../../../clucene/src/CLucene/util/BitSet.cpp \
    ../../../../clucene/src/CLucene/search/BooleanQuery.cpp \
    ../../../../clucene/src/CLucene/search/BooleanScorer.cpp \
    ../../../../clucene/src/CLucene/index/CompoundFile.cpp \
    ../../../../clucene/src/CLucene/debug/condition.cpp \
    ../../../../clucene/src/CLucene/search/ConjunctionScorer.cpp \
    ../../../../clucene/src/CLucene/document/Document.cpp \
    ../../../../clucene/src/CLucene/index/DocumentWriter.cpp \
    ../../../../clucene/src/CLucene/util/Equators.cpp \
    ../../../../clucene/src/CLucene/debug/error.cpp \
    ../../../../clucene/src/CLucene/search/ExactPhraseScorer.cpp \
    ../../../../clucene/src/CLucene/search/Explanation.cpp \
    ../../../../clucene/src/CLucene/util/FastCharStream.cpp \
    ../../../../clucene/src/CLucene/document/Field.cpp \
    ../../../../clucene/src/CLucene/search/FieldCache.cpp \
    ../../../../clucene/src/CLucene/search/FieldCacheImpl.cpp \
    ../../../../clucene/src/CLucene/search/FieldDocSortedHitQueue.cpp \
    ../../../../clucene/src/CLucene/index/FieldInfos.cpp \
    ../../../../clucene/src/CLucene/search/FieldSortedHitQueue.cpp \
    ../../../../clucene/src/CLucene/index/FieldsReader.cpp \
    ../../../../clucene/src/CLucene/index/FieldsWriter.cpp \
    ../../../../clucene/src/CLucene/util/fileinputstream.cpp \
    ../../../../clucene/src/CLucene/search/FilteredTermEnum.cpp \
    ../../../../clucene/src/CLucene/store/FSDirectory.cpp \
    ../../../../clucene/src/CLucene/search/FuzzyQuery.cpp \
    ../../../../clucene/src/CLucene/config/gunichartables.cpp \
    ../../../../clucene/src/CLucene/search/HitQueue.cpp \
    ../../../../clucene/src/CLucene/search/Hits.cpp \
    ../../../../clucene/src/CLucene/store/IndexInput.cpp \
    ../../../../clucene/src/CLucene/store/IndexOutput.cpp \
    ../../../../clucene/src/CLucene/index/IndexReader.cpp \
    ../../../../clucene/src/CLucene/search/IndexSearcher.cpp \
    ../../../../clucene/src/CLucene/index/IndexWriter.cpp \
    ../../../../clucene/src/CLucene/queryParser/Lexer.cpp \
    ../../../../clucene/src/CLucene/store/Lock.cpp \
    ../../../../clucene/src/CLucene/debug/memtracking.cpp \
    ../../../../clucene/src/CLucene/util/Misc.cpp \
    ../../../../clucene/src/CLucene/index/MultiReader.cpp \
    ../../../../clucene/src/CLucene/search/MultiTermQuery.cpp \
    ../../../../clucene/src/CLucene/search/PhrasePositions.cpp \
    ../../../../clucene/src/CLucene/search/PhraseQuery.cpp \
    ../../../../clucene/src/CLucene/search/PhraseScorer.cpp \
    ../../../../clucene/src/CLucene/search/PrefixQuery.cpp \
    ../../../../clucene/src/CLucene/queryParser/QueryParser.cpp \
    ../../../../clucene/src/CLucene/queryParser/QueryParserBase.cpp \
    ../../../../clucene/src/CLucene/queryParser/QueryToken.cpp \
    ../../../../clucene/src/CLucene/store/RAMDirectory.cpp \
    ../../../../clucene/src/CLucene/search/RangeQuery.cpp \
    ../../../../clucene/src/CLucene/util/Reader.cpp \
    ../../../../clucene/src/CLucene/config/repl_tcscasecmp.cpp \
    ../../../../clucene/src/CLucene/config/repl_tcslwr.cpp \
    ../../../../clucene/src/CLucene/search/SearchHeader.cpp \
    ../../../../clucene/src/CLucene/index/SegmentInfos.cpp \
    ../../../../clucene/src/CLucene/index/SegmentMergeInfo.cpp \
    ../../../../clucene/src/CLucene/index/SegmentMergeQueue.cpp \
    ../../../../clucene/src/CLucene/index/SegmentMerger.cpp \
    ../../../../clucene/src/CLucene/index/SegmentReader.cpp \
    ../../../../clucene/src/CLucene/index/SegmentTermDocs.cpp \
    ../../../../clucene/src/CLucene/index/SegmentTermEnum.cpp \
    ../../../../clucene/src/CLucene/index/SegmentTermPositions.cpp \
    ../../../../clucene/src/CLucene/index/SegmentTermVector.cpp \
    ../../../../clucene/src/CLucene/search/Similarity.cpp \
    ../../../../clucene/src/CLucene/search/SloppyPhraseScorer.cpp \
    ../../../../clucene/src/CLucene/search/Sort.cpp \
    ../../../../clucene/src/CLucene/analysis/standard/StandardAnalyzer.cpp \
    ../../../../clucene/src/CLucene/analysis/standard/StandardFilter.cpp \
    ../../../../clucene/src/CLucene/analysis/standard/StandardTokenizer.cpp \
    ../../../../clucene/src/CLucene/StdHeader.cpp \
    ../../../../clucene/src/CLucene/util/StringBuffer.cpp \
    ../../../../clucene/src/CLucene/util/StringIntern.cpp \
    ../../../../clucene/src/CLucene/index/Term.cpp \
    ../../../../clucene/src/CLucene/index/TermInfo.cpp \
    ../../../../clucene/src/CLucene/index/TermInfosReader.cpp \
    ../../../../clucene/src/CLucene/index/TermInfosWriter.cpp \
    ../../../../clucene/src/CLucene/search/TermQuery.cpp \
    ../../../../clucene/src/CLucene/search/TermScorer.cpp \
    ../../../../clucene/src/CLucene/index/TermVectorReader.cpp \
    ../../../../clucene/src/CLucene/index/TermVectorWriter.cpp \
    ../../../../clucene/src/CLucene/util/ThreadLocal.cpp \
    ../../../../clucene/src/CLucene/config/threads.cpp \
    ../../../../clucene/src/CLucene/queryParser/TokenList.cpp \
    ../../../../clucene/src/CLucene/store/TransactionalRAMDirectory.cpp \
    ../../../../clucene/src/CLucene/config/utf8.cpp \
    ../../../../clucene/src/CLucene/search/WildcardQuery.cpp \
    ../../../../clucene/src/CLucene/search/WildcardTermEnum.cpp \
    ../../../../clucene/src/CLucene/config/repl_tcstoll.cpp \
    ../../../../clucene/src/CLucene/config/repl_lltot.cpp \
    ../../../../clucene/src/CLucene/config/repl_tprintf.cpp

OTHER_FILES += \
    android/AndroidManifest.xml \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/layout/splash.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/version.xml \
    android/AndroidManifest.xml \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/layout/splash.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/version.xml
