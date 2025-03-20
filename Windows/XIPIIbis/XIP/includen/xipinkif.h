#ifndef XIPINKIF_H
#define XIPINKIF_H

KifElement* PreParse(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreParseFile(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreParseXMLFile(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreExtract(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreCreateDependency(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreSetFeature(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreRemoveFeature(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreFeature(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreGetData(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreGetName(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreDependencyStack(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreDependencyParameters(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreXipNumber(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreOffset(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreOffsetChar(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreTokenOffset(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreLeftOffset(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreRightOffset(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreLeftOffsetChar(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreRightOffsetChar(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreLeftTokenOffset(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreRightTokenOffset(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreLemma(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreSurface(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreXmlNode(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreNext(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PrePrevious(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreParent(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreChild(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreReplace(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreReplaceRgx(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreLoad(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PrePush(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PrePop(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreLast(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreSet(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreGet(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreCompilenet(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreCompilenetwith(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreTokup(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreTokdown(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreParseAddOption(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreParseRemoveOption(KifElement* pattern,KifDomain* dom,KifCallFunction* callfunc);

KifElement* PreCompilelineInit(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreCompileline(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc);
KifElement* PreCompilelineSave(KifElement* contextualpattern,KifDomain* dom,KifCallFunction* callfunc);

#endif
