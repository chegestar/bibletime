# Configuration
VERSION = 0.9.0

CONFIG += clucene
CONFIG += webkit

#CLUCENE_PATH = ../../../../clucene/src
CLUCENE_PATH = C:\Qt\Qt5.0.2\5.0.2\Src\qttools\src\assistant\3rdparty\clucene\src

SWORD_PATH = ../../../../sword

# Base section, nothing platform specific
DEFINES += BT_MINI

INCLUDEPATH += . \
    $${SWORD_PATH}/include \
    ../../../../bt/src/frontend-mini \
    ../../../../bt/src

SOURCES += \
    $${SWORD_PATH}/src/modules/common/zipcomprs.cpp \
    $${SWORD_PATH}/src/utilfuns/zlib/untgz.c \
    $${SWORD_PATH}/src/utilfuns/zlib/gzio.c \
    $${SWORD_PATH}/src/utilfuns/zlib/zutil.c \
    $${SWORD_PATH}/src/utilfuns/zlib/uncompr.c \
    $${SWORD_PATH}/src/utilfuns/zlib/trees.c \
    $${SWORD_PATH}/src/utilfuns/zlib/infutil.c \
    $${SWORD_PATH}/src/utilfuns/zlib/inftrees.c \
    $${SWORD_PATH}/src/utilfuns/zlib/inflate.c \
    $${SWORD_PATH}/src/utilfuns/zlib/inffast.c \
    $${SWORD_PATH}/src/utilfuns/zlib/infcodes.c \
    $${SWORD_PATH}/src/utilfuns/zlib/infblock.c \
    $${SWORD_PATH}/src/utilfuns/zlib/deflate.c \
    $${SWORD_PATH}/src/utilfuns/zlib/crc32.c \
    $${SWORD_PATH}/src/utilfuns/zlib/compress.c \
    $${SWORD_PATH}/src/utilfuns/zlib/adler32.c \
    $${SWORD_PATH}/src/mgr/ftplibftpt.cpp \
    $${SWORD_PATH}/src/utilfuns/ftplib.c \
    $${SWORD_PATH}/src/utilfuns/regex.c \
    $${SWORD_PATH}/src/frontend/swdisp.cpp \
    $${SWORD_PATH}/src/frontend/swlog.cpp \
    $${SWORD_PATH}/src/keys/swkey.cpp \
    $${SWORD_PATH}/src/keys/listkey.cpp \
    $${SWORD_PATH}/src/keys/strkey.cpp \
    $${SWORD_PATH}/src/keys/treekey.cpp \
    $${SWORD_PATH}/src/keys/treekeyidx.cpp \
    $${SWORD_PATH}/src/keys/versekey.cpp \
    $${SWORD_PATH}/src/keys/versetreekey.cpp \
    $${SWORD_PATH}/src/mgr/swconfig.cpp \
    $${SWORD_PATH}/src/mgr/swmgr.cpp \
    $${SWORD_PATH}/src/mgr/swfiltermgr.cpp \
    $${SWORD_PATH}/src/mgr/encfiltmgr.cpp \
    $${SWORD_PATH}/src/mgr/markupfiltmgr.cpp \
    $${SWORD_PATH}/src/mgr/filemgr.cpp \
    $${SWORD_PATH}/src/mgr/versificationmgr.cpp \
    $${SWORD_PATH}/src/mgr/ftptrans.cpp \
    $${SWORD_PATH}/src/mgr/swlocale.cpp \
    $${SWORD_PATH}/src/mgr/localemgr.cpp \
    $${SWORD_PATH}/src/mgr/swcacher.cpp \
    $${SWORD_PATH}/src/mgr/swsearchable.cpp \
    $${SWORD_PATH}/src/mgr/installmgr.cpp \
    $${SWORD_PATH}/src/mgr/stringmgr.cpp \
    $${SWORD_PATH}/src/modules/swmodule.cpp \
    $${SWORD_PATH}/src/modules/comments/swcom.cpp \
    $${SWORD_PATH}/src/modules/comments/hrefcom/hrefcom.cpp \
    $${SWORD_PATH}/src/modules/comments/rawcom/rawcom.cpp \
    $${SWORD_PATH}/src/modules/comments/rawcom4/rawcom4.cpp \
    $${SWORD_PATH}/src/modules/comments/rawfiles/rawfiles.cpp \
    $${SWORD_PATH}/src/modules/comments/zcom/zcom.cpp \
    $${SWORD_PATH}/src/modules/common/rawstr.cpp \
    $${SWORD_PATH}/src/modules/common/rawstr4.cpp \
    $${SWORD_PATH}/src/modules/common/swcomprs.cpp \
    $${SWORD_PATH}/src/modules/common/lzsscomprs.cpp \
    $${SWORD_PATH}/src/modules/common/rawverse.cpp \
    $${SWORD_PATH}/src/modules/common/rawverse4.cpp \
    $${SWORD_PATH}/src/modules/common/swcipher.cpp \
    $${SWORD_PATH}/src/modules/common/zverse.cpp \
    $${SWORD_PATH}/src/modules/common/zstr.cpp \
    $${SWORD_PATH}/src/modules/common/entriesblk.cpp \
    $${SWORD_PATH}/src/modules/common/sapphire.cpp \
    $${SWORD_PATH}/src/modules/filters/swbasicfilter.cpp \
    $${SWORD_PATH}/src/modules/filters/swoptfilter.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfhtml.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfxhtml.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfhtmlhref.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfwebif.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfplain.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfrtf.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfstrongs.cpp \
    $${SWORD_PATH}/src/modules/filters/gbffootnotes.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfheadings.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfredletterwords.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfmorph.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfwordjs.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlstrongs.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlfootnotes.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlheadings.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlmorph.cpp \
    $${SWORD_PATH}/src/modules/filters/thmllemma.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlscripref.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlvariants.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlgbf.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlrtf.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlhtml.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlxhtml.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlhtmlhref.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlwebif.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlwordjs.cpp \
    $${SWORD_PATH}/src/modules/filters/teiplain.cpp \
    $${SWORD_PATH}/src/modules/filters/teirtf.cpp \
    $${SWORD_PATH}/src/modules/filters/teihtmlhref.cpp \
    $${SWORD_PATH}/src/modules/filters/teixhtml.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfthml.cpp \
    $${SWORD_PATH}/src/modules/filters/gbfosis.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlosis.cpp \
    $${SWORD_PATH}/src/modules/filters/thmlplain.cpp \
    $${SWORD_PATH}/src/modules/filters/osisosis.cpp \
    $${SWORD_PATH}/src/modules/filters/osisheadings.cpp \
    $${SWORD_PATH}/src/modules/filters/osisfootnotes.cpp \
    $${SWORD_PATH}/src/modules/filters/osishtmlhref.cpp \
    $${SWORD_PATH}/src/modules/filters/osisxhtml.cpp \
    $${SWORD_PATH}/src/modules/filters/osiswebif.cpp \
    $${SWORD_PATH}/src/modules/filters/osismorph.cpp \
    $${SWORD_PATH}/src/modules/filters/osisstrongs.cpp \
    $${SWORD_PATH}/src/modules/filters/osisplain.cpp \
    $${SWORD_PATH}/src/modules/filters/osisrtf.cpp \
    $${SWORD_PATH}/src/modules/filters/osislemma.cpp \
    $${SWORD_PATH}/src/modules/filters/osisredletterwords.cpp \
    $${SWORD_PATH}/src/modules/filters/osisscripref.cpp \
    $${SWORD_PATH}/src/modules/filters/osisvariants.cpp \
    $${SWORD_PATH}/src/modules/filters/osiswordjs.cpp \
    $${SWORD_PATH}/src/modules/filters/osismorphsegmentation.cpp \
    $${SWORD_PATH}/src/modules/filters/osisruby.cpp \
    $${SWORD_PATH}/src/modules/filters/latin1utf8.cpp \
    $${SWORD_PATH}/src/modules/filters/latin1utf16.cpp \
    $${SWORD_PATH}/src/modules/filters/utf8utf16.cpp \
    $${SWORD_PATH}/src/modules/filters/utf16utf8.cpp \
    $${SWORD_PATH}/src/modules/filters/utf8html.cpp \
    $${SWORD_PATH}/src/modules/filters/utf8latin1.cpp \
    $${SWORD_PATH}/src/modules/filters/utf8cantillation.cpp \
    $${SWORD_PATH}/src/modules/filters/utf8hebrewpoints.cpp \
    $${SWORD_PATH}/src/modules/filters/utf8arabicpoints.cpp \
    $${SWORD_PATH}/src/modules/filters/utf8greekaccents.cpp \
    $${SWORD_PATH}/src/modules/filters/cipherfil.cpp \
    $${SWORD_PATH}/src/modules/filters/rtfhtml.cpp \
    $${SWORD_PATH}/src/modules/filters/plainfootnotes.cpp \
    $${SWORD_PATH}/src/modules/filters/plainhtml.cpp \
    $${SWORD_PATH}/src/modules/filters/greeklexattribs.cpp \
    $${SWORD_PATH}/src/modules/filters/unicodertf.cpp \
    $${SWORD_PATH}/src/modules/filters/papyriplain.cpp \
    $${SWORD_PATH}/src/modules/genbook/swgenbook.cpp \
    $${SWORD_PATH}/src/modules/genbook/rawgenbook/rawgenbook.cpp \
    $${SWORD_PATH}/src/modules/lexdict/swld.cpp \
    $${SWORD_PATH}/src/modules/lexdict/rawld/rawld.cpp \
    $${SWORD_PATH}/src/modules/lexdict/rawld4/rawld4.cpp \
    $${SWORD_PATH}/src/modules/lexdict/zld/zld.cpp \
    $${SWORD_PATH}/src/modules/texts/swtext.cpp \
    $${SWORD_PATH}/src/modules/texts/rawtext/rawtext.cpp \
    $${SWORD_PATH}/src/modules/texts/rawtext4/rawtext4.cpp \
    $${SWORD_PATH}/src/modules/texts/ztext/ztext.cpp \
    $${SWORD_PATH}/src/utilfuns/swobject.cpp \
    $${SWORD_PATH}/src/utilfuns/utilstr.cpp \
    $${SWORD_PATH}/src/utilfuns/utilxml.cpp \
    $${SWORD_PATH}/src/utilfuns/swunicod.cpp \
    $${SWORD_PATH}/src/utilfuns/swversion.cpp \
    $${SWORD_PATH}/src/utilfuns/swbuf.cpp \
    $${SWORD_PATH}/src/utilfuns/ftpparse.c \
    $${SWORD_PATH}/src/utilfuns/url.cpp \
    $${SWORD_PATH}/src/utilfuns/roman.cpp \
    ../../../../bt/src/frontend-mini/models/btminimoduletextmodel.cpp \
    ../../../../bt/src/frontend-mini/models/btminimodulenavigationmodel.cpp \
    ../../../../bt/src/frontend-mini/models/btminimodelsmodel.cpp \
    ../../../../bt/src/frontend-mini/models/btminisettingsmodel.cpp \
    ../../../../bt/src/frontend-mini/ui/btministyle.cpp \
    ../../../../bt/src/frontend-mini/ui/btminipanel.cpp \
    ../../../../bt/src/frontend-mini/ui/btminimenu.cpp \
    ../../../../bt/src/frontend-mini/view/btminiview.cpp \
    ../../../../bt/src/frontend-mini/view/btminilayoutdelegate.cpp \
    ../../../../bt/src/frontend-mini/btmini.cpp \
    ../../../../bt/src/backend/cswordmodulesearch.cpp \
    ../../../../bt/src/backend/btmoduletreeitem.cpp \
    ../../../../bt/src/backend/btinstallbackend.cpp \
    ../../../../bt/src/backend/managers/referencemanager.cpp \
    ../../../../bt/src/backend/managers/cswordbackend.cpp \
    ../../../../bt/src/backend/managers/clanguagemgr.cpp \
    ../../../../bt/src/backend/managers/cdisplaytemplatemgr.cpp \
    ../../../../bt/src/backend/managers/btstringmgr.cpp \
    ../../../../bt/src/util/directory.cpp \
    ../../../../bt/src/util/cresmgr.cpp \
    ../../../../bt/src/backend/config/cbtconfig.cpp \
    ../../../../bt/src/backend/bookshelfmodel/btbookshelftreemodel.cpp \
    ../../../../bt/src/frontend/bookshelfmanager/btinstallmgr.cpp \
    ../../../../bt/src/backend/keys/cswordversekey.cpp \
    ../../../../bt/src/backend/keys/cswordldkey.cpp \
    ../../../../bt/src/backend/keys/cswordkey.cpp \
    ../../../../bt/src/backend/drivers/cswordmoduleinfo.cpp \
    ../../../../bt/src/backend/drivers/cswordlexiconmoduleinfo.cpp \
    ../../../../bt/src/backend/drivers/cswordcommentarymoduleinfo.cpp \
    ../../../../bt/src/backend/drivers/cswordbiblemoduleinfo.cpp \
    ../../../../bt/src/backend/rendering/ctextrendering.cpp \
    ../../../../bt/src/backend/rendering/chtmlexportrendering.cpp \
    ../../../../bt/src/backend/rendering/centrydisplay.cpp \
    ../../../../bt/src/backend/rendering/cdisplayrendering.cpp \
    ../../../../bt/src/backend/bookshelfmodel/moduleitem.cpp \
    ../../../../bt/src/backend/bookshelfmodel/languageitem.cpp \
    ../../../../bt/src/backend/bookshelfmodel/item.cpp \
    ../../../../bt/src/backend/bookshelfmodel/categoryitem.cpp \
    ../../../../bt/src/frontend/cinfodisplay.cpp \
    ../../../../bt/src/frontend/bookshelfmanager/installpage/btinstallthread.cpp \
    ../../../../bt/src/util/tool.cpp \
    ../../../../bt/src/util/dialogutil.cpp \
    ../../../../bt/src/backend/rendering/cchapterdisplay.cpp \
    ../../../../bt/src/backend/rendering/cbookdisplay.cpp \
    ../../../../bt/src/backend/filters/thmltohtml.cpp \
    ../../../../bt/src/backend/filters/teitohtml.cpp \
    ../../../../bt/src/backend/filters/plaintohtml.cpp \
    ../../../../bt/src/backend/filters/osistohtml.cpp \
    ../../../../bt/src/backend/filters/gbftohtml.cpp \
    ../../../../bt/src/backend/bookshelfmodel/btbookshelfmodel.cpp \
    ../../../../bt/src/backend/filters/thmltoplain.cpp \
    ../../../../bt/src/backend/drivers/cswordbookmoduleinfo.cpp \
    ../../../../bt/src/frontend/displaywindow/btactioncollection.cpp \
    ../../../../bt/src/backend/bookshelfmodel/indexingitem.cpp \
    ../../../../bt/src/backend/keys/cswordtreekey.cpp \
    ../../../../bt/src/frontend/crossrefrendering.cpp \
    ../../../../bt/src/backend/filters/osismorphsegmentation_bt.cpp


HEADERS += \
    ../../../../bt/src/frontend-mini/models/btminimoduletextmodel.h \
    ../../../../bt/src/frontend-mini/models/btminimodulenavigationmodel.h \
    ../../../../bt/src/frontend-mini/models/btminimodelsmodel.h \
    ../../../../bt/src/frontend-mini/models/btminisettingsmodel.h \
    ../../../../bt/src/frontend-mini/ui/btminipanel.h \
    ../../../../bt/src/frontend-mini/ui/btminimenu.h \
    ../../../../bt/src/frontend-mini/view/btminiview.h \
    ../../../../bt/src/frontend-mini/view/btminilayoutdelegate.h \
    ../../../../bt/src/backend/cswordmodulesearch.h \
    ../../../../bt/src/backend/btmoduletreeitem.h \
    ../../../../bt/src/backend/btinstallbackend.h \
    ../../../../bt/src/backend/managers/referencemanager.h \
    ../../../../bt/src/backend/managers/cswordbackend.h \
    ../../../../bt/src/backend/managers/clanguagemgr.h \
    ../../../../bt/src/backend/managers/cdisplaytemplatemgr.h \
    ../../../../bt/src/backend/managers/btstringmgr.h \
    ../../../../bt/src/util/directory.h \
    ../../../../bt/src/util/cresmgr.h \
    ../../../../bt/src/backend/config/cbtconfig.h \
    ../../../../bt/src/backend/bookshelfmodel/btbookshelftreemodel.h \
    ../../../../bt/src/frontend/bookshelfmanager/btinstallmgr.h \
    ../../../../bt/src/frontend/bookshelfmanager/installpage/btinstallthread.h \
    ../../../../bt/src/frontend/cinfodisplay.h \
    ../../../../bt/src/frontend/displaywindow/btactioncollection.h \
    ../../../../bt/src/frontend/crossrefrendering.h \
    ../../../../bt/src/backend/keys/cswordversekey.h \
    ../../../../bt/src/backend/keys/cswordldkey.h \
    ../../../../bt/src/backend/keys/cswordkey.h \
    ../../../../bt/src/backend/keys/cswordtreekey.h \
    ../../../../bt/src/backend/drivers/cswordmoduleinfo.h \
    ../../../../bt/src/backend/drivers/cswordlexiconmoduleinfo.h \
    ../../../../bt/src/backend/drivers/cswordcommentarymoduleinfo.h \
    ../../../../bt/src/backend/drivers/cswordbiblemoduleinfo.h \
    ../../../../bt/src/backend/rendering/ctextrendering.h \
    ../../../../bt/src/backend/rendering/chtmlexportrendering.h \
    ../../../../bt/src/backend/rendering/centrydisplay.h \
    ../../../../bt/src/backend/rendering/cdisplayrendering.h \
    ../../../../bt/src/backend/rendering/cplaintextexportrendering.h \
    ../../../../bt/src/backend/bookshelfmodel/moduleitem.h \
    ../../../../bt/src/backend/bookshelfmodel/languageitem.h \
    ../../../../bt/src/backend/bookshelfmodel/item.h \
    ../../../../bt/src/backend/bookshelfmodel/categoryitem.h \
    ../../../../bt/src/util/tool.h \
    ../../../../bt/src/util/dialogutil.h \
    ../../../../bt/src/util/btsignal.h \
    ../../../../bt/src/backend/rendering/cchapterdisplay.h \
    ../../../../bt/src/backend/rendering/cbookdisplay.h \
    ../../../../bt/src/backend/filters/thmltohtml.h \
    ../../../../bt/src/backend/filters/teitohtml.h \
    ../../../../bt/src/backend/filters/plaintohtml.h \
    ../../../../bt/src/backend/filters/osistohtml.h \
    ../../../../bt/src/backend/filters/gbftohtml.h \
    ../../../../bt/src/backend/bookshelfmodel/btbookshelfmodel.h \
    ../../../../bt/src/backend/filters/thmltoplain.h \
    ../../../../bt/src/backend/drivers/cswordbookmoduleinfo.h \
    ../../../../bt/src/backend/bookshelfmodel/indexingitem.h \
    ../../../../bt/src/backend/keys/cswordtreekey.h \
    ../../../../bt/src/frontend/crossrefrendering.h \
    ../../../../bt/src/backend/filters/osismorphsegmentation.h \
    ../../../../bt/src/frontend-mini/btmini.h \
    ../../../../bt/src/frontend-mini/ui/btministyle.cpp \

RESOURCES += \
    ../../../../bt/src/frontend-mini/ui/btministyle.qrc \
    ../../../btmini.qrc

OTHER_FILES += ../../../src/frontend-mini/todo.txt;
# BtMini translations
TRANSLATIONS += ../../../src/frontend-mini/translations/bibletimemini_ru.ts

# BibleTime translations
TRANSLATIONS += ../../../i18n/messages/bibletime_ui_ru.ts


# WebKit
webkit {
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += webkitwidgets
}
else {
    QT += webkit
}
DEFINES += BT_MINI_WEBKIT
}


# Clucene
clucene {
DEFINES += _UCS2 _CL_DISABLE_MULTITHREADING

INCLUDEPATH += $${CLUCENE_PATH}

SOURCES += \
    $${CLUCENE_PATH}/CLucene/analysis/AnalysisHeader.cpp \
    $${CLUCENE_PATH}/CLucene/analysis/Analyzers.cpp \
    $${CLUCENE_PATH}/CLucene/util/BitSet.cpp \
    $${CLUCENE_PATH}/CLucene/search/BooleanQuery.cpp \
    $${CLUCENE_PATH}/CLucene/search/BooleanScorer.cpp \
    $${CLUCENE_PATH}/CLucene/index/CompoundFile.cpp \
    $${CLUCENE_PATH}/CLucene/debug/condition.cpp \
    $${CLUCENE_PATH}/CLucene/search/ConjunctionScorer.cpp \
    $${CLUCENE_PATH}/CLucene/document/Document.cpp \
    $${CLUCENE_PATH}/CLucene/index/DocumentWriter.cpp \
    $${CLUCENE_PATH}/CLucene/util/Equators.cpp \
    $${CLUCENE_PATH}/CLucene/debug/error.cpp \
    $${CLUCENE_PATH}/CLucene/search/ExactPhraseScorer.cpp \
    $${CLUCENE_PATH}/CLucene/search/Explanation.cpp \
    $${CLUCENE_PATH}/CLucene/util/FastCharStream.cpp \
    $${CLUCENE_PATH}/CLucene/document/Field.cpp \
    $${CLUCENE_PATH}/CLucene/search/FieldCache.cpp \
    $${CLUCENE_PATH}/CLucene/search/FieldCacheImpl.cpp \
    $${CLUCENE_PATH}/CLucene/search/FieldDocSortedHitQueue.cpp \
    $${CLUCENE_PATH}/CLucene/index/FieldInfos.cpp \
    $${CLUCENE_PATH}/CLucene/search/FieldSortedHitQueue.cpp \
    $${CLUCENE_PATH}/CLucene/index/FieldsReader.cpp \
    $${CLUCENE_PATH}/CLucene/index/FieldsWriter.cpp \
    $${CLUCENE_PATH}/CLucene/util/fileinputstream.cpp \
    $${CLUCENE_PATH}/CLucene/search/FilteredTermEnum.cpp \
    $${CLUCENE_PATH}/CLucene/store/FSDirectory.cpp \
    $${CLUCENE_PATH}/CLucene/search/FuzzyQuery.cpp \
    $${CLUCENE_PATH}/CLucene/config/gunichartables.cpp \
    $${CLUCENE_PATH}/CLucene/search/HitQueue.cpp \
    $${CLUCENE_PATH}/CLucene/search/Hits.cpp \
    $${CLUCENE_PATH}/CLucene/store/IndexInput.cpp \
    $${CLUCENE_PATH}/CLucene/store/IndexOutput.cpp \
    $${CLUCENE_PATH}/CLucene/index/IndexReader.cpp \
    $${CLUCENE_PATH}/CLucene/search/IndexSearcher.cpp \
    $${CLUCENE_PATH}/CLucene/index/IndexWriter.cpp \
    $${CLUCENE_PATH}/CLucene/queryParser/Lexer.cpp \
    $${CLUCENE_PATH}/CLucene/store/Lock.cpp \
    $${CLUCENE_PATH}/CLucene/debug/memtracking.cpp \
    $${CLUCENE_PATH}/CLucene/util/Misc.cpp \
    $${CLUCENE_PATH}/CLucene/index/MultiReader.cpp \
    $${CLUCENE_PATH}/CLucene/search/MultiTermQuery.cpp \
    $${CLUCENE_PATH}/CLucene/search/PhrasePositions.cpp \
    $${CLUCENE_PATH}/CLucene/search/PhraseQuery.cpp \
    $${CLUCENE_PATH}/CLucene/search/PhraseScorer.cpp \
    $${CLUCENE_PATH}/CLucene/search/PrefixQuery.cpp \
    $${CLUCENE_PATH}/CLucene/queryParser/QueryParser.cpp \
    $${CLUCENE_PATH}/CLucene/queryParser/QueryParserBase.cpp \
    $${CLUCENE_PATH}/CLucene/queryParser/QueryToken.cpp \
    $${CLUCENE_PATH}/CLucene/store/RAMDirectory.cpp \
    $${CLUCENE_PATH}/CLucene/search/RangeQuery.cpp \
    $${CLUCENE_PATH}/CLucene/util/Reader.cpp \
    $${CLUCENE_PATH}/CLucene/config/repl_tcscasecmp.cpp \
    $${CLUCENE_PATH}/CLucene/config/repl_tcslwr.cpp \
    $${CLUCENE_PATH}/CLucene/search/SearchHeader.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentInfos.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentMergeInfo.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentMergeQueue.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentMerger.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentReader.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentTermDocs.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentTermEnum.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentTermPositions.cpp \
    $${CLUCENE_PATH}/CLucene/index/SegmentTermVector.cpp \
    $${CLUCENE_PATH}/CLucene/search/Similarity.cpp \
    $${CLUCENE_PATH}/CLucene/search/SloppyPhraseScorer.cpp \
    $${CLUCENE_PATH}/CLucene/search/Sort.cpp \
    $${CLUCENE_PATH}/CLucene/analysis/standard/StandardAnalyzer.cpp \
    $${CLUCENE_PATH}/CLucene/analysis/standard/StandardFilter.cpp \
    $${CLUCENE_PATH}/CLucene/analysis/standard/StandardTokenizer.cpp \
    $${CLUCENE_PATH}/CLucene/StdHeader.cpp \
    $${CLUCENE_PATH}/CLucene/util/StringBuffer.cpp \
    $${CLUCENE_PATH}/CLucene/util/StringIntern.cpp \
    $${CLUCENE_PATH}/CLucene/index/Term.cpp \
    $${CLUCENE_PATH}/CLucene/index/TermInfo.cpp \
    $${CLUCENE_PATH}/CLucene/index/TermInfosReader.cpp \
    $${CLUCENE_PATH}/CLucene/index/TermInfosWriter.cpp \
    $${CLUCENE_PATH}/CLucene/search/TermQuery.cpp \
    $${CLUCENE_PATH}/CLucene/search/TermScorer.cpp \
    $${CLUCENE_PATH}/CLucene/index/TermVectorReader.cpp \
    $${CLUCENE_PATH}/CLucene/index/TermVectorWriter.cpp \
    $${CLUCENE_PATH}/CLucene/util/ThreadLocal.cpp \
    $${CLUCENE_PATH}/CLucene/config/threads.cpp \
    $${CLUCENE_PATH}/CLucene/queryParser/TokenList.cpp \
    $${CLUCENE_PATH}/CLucene/store/TransactionalRAMDirectory.cpp \
    $${CLUCENE_PATH}/CLucene/config/utf8.cpp \
    $${CLUCENE_PATH}/CLucene/search/WildcardQuery.cpp \
    $${CLUCENE_PATH}/CLucene/search/WildcardTermEnum.cpp \
    $${CLUCENE_PATH}/CLucene/config/repl_tcstoll.cpp \
    $${CLUCENE_PATH}/CLucene/config/repl_lltot.cpp \
    $${CLUCENE_PATH}/CLucene/config/repl_tprintf.cpp
}
else {
DEFINES += BT_NO_CLUCENE
}


# Platforms section
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}

!symbian {
	INCLUDEPATH += $${SWORD_PATH}/include/internal/regex
}

win32 {
INCLUDEPATH += $${SWORD_PATH}/src/utilfuns/win32

SOURCES += $${SWORD_PATH}/src/utilfuns/win32/dirent.cpp

LIBS += -lws2_32
}

# Android
android {
OTHER_FILES += android/AndroidManifest.xml \
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
}
else {
# MeeGo platform
unix {
include(deployment.pri)
qtcAddDeployment()

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog

contains(MEEGO_EDITION,harmattan) {
    target.path = /opt/btmini/bin
    INSTALLS += target
}
}
}

# Symbian platform
symbian {
TARGET.UID3 = 0xE5723167
TARGET.CAPABILITY += NetworkServices
TARGET.EPOCSTACKSIZE = 0x14000
TARGET.EPOCHEAPSIZE = 0x020000 0x2000000

DEPLOYMENT.display_name = BibleTime Mini

packageheader = "$${LITERAL_HASH}{\"BibleTime Mini\"}, (0xE5723167), 0, 3, 4, TYPE=SA"

vendorinfo = \
"%{\"Crosswire\"}" \
":\"Crosswire\""

mini_deployment.pkg_prerules = packageheader vendorinfo

DEPLOYMENT += mini_deployment

clucene {
DEFINES += __GNUC__ NO_DUMMY_DECL
}
}

