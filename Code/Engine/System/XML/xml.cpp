#include "xml.h"

inline void _not_implemented(const char* file, int line, const char* fn)
{
	CryError("%s:%d:%s", file, line, fn);
}

#define NOT_IMPLEMENTED _not_implemented(__FILE__, __LINE__, __FUNCTION__)

int CIXMLDOMNode::AddRef()
{
	NOT_IMPLEMENTED;
}

void CIXMLDOMNode::Release()
{
	NOT_IMPLEMENTED;
}

XDOM::_DOMNodeType CIXMLDOMNode::getNodeType()
{
	NOT_IMPLEMENTED;
}

const char* CIXMLDOMNode::getText()
{
	NOT_IMPLEMENTED;
}

const char* CIXMLDOMNode::getName()
{
	NOT_IMPLEMENTED;
}

XDOM::IXMLDOMNodeList* CIXMLDOMNode::getChildNodes()
{
	NOT_IMPLEMENTED;
}

void CIXMLDOMNode::setText(const char* sText)
{
	NOT_IMPLEMENTED;
}

void CIXMLDOMNode::setName(const char* sName)
{
	NOT_IMPLEMENTED;
}

bool CIXMLDOMNode::hasChildNodes()
{
	NOT_IMPLEMENTED;
}

bool CIXMLDOMNode::appendChild(IXMLDOMNode* pNode)
{
	NOT_IMPLEMENTED;
}

XDOM::IXMLDOMNode* CIXMLDOMNode::getAttribute(const char* sName)
{
	NOT_IMPLEMENTED;
}

XDOM::IXMLDOMNodeList* CIXMLDOMNode::getElementsByTagName(const char* sName)
{
	NOT_IMPLEMENTED;
}

int CXMLDOMNodeList::AddRef()
{
	return ++m_nRefCounter;
}

void CXMLDOMNodeList::Release()
{
	++m_nRefCounter;
}

CXMLDOMNodeList::CXMLDOMNodeList(tinyxml2::XMLNode* first)
    : m_NodeList(first)
{
}

size_t CXMLDOMNodeList::length()
{
	NOT_IMPLEMENTED;
	return 0;
}

void CXMLDOMNodeList::reset()
{
	NOT_IMPLEMENTED;
}

XDOM::IXMLDOMNode* CXMLDOMNodeList::nextNode()
{
	//return new CXMLDOMNodeList(m_NodeList->NextSibling());
	NOT_IMPLEMENTED;
	return nullptr;
}

// Inherited via IXMLDOMDocument

int CXMLDocument::AddRef()
{
	_reference_target_t::AddRef();
	return NumRefs();
}

void CXMLDocument::Release()
{
	_reference_target_t::Release();
}

XDOM::_DOMNodeType CXMLDocument::getNodeType()
{
	NOT_IMPLEMENTED;
	return XDOM::_DOMNodeType();
}

const char* CXMLDocument::getText()
{
	NOT_IMPLEMENTED;
	return nullptr;
}

const char* CXMLDocument::getName()
{
	NOT_IMPLEMENTED;
	return nullptr;
}

XDOM::IXMLDOMNodeList* CXMLDocument::getChildNodes()
{
	NOT_IMPLEMENTED;
	return new CXMLDOMNodeList(m_Document.FirstChild());
}

void CXMLDocument::setText(const char* sText)
{
	NOT_IMPLEMENTED;
}

void CXMLDocument::setName(const char* sName)
{
	NOT_IMPLEMENTED;
}

bool CXMLDocument::hasChildNodes()
{
	return !m_Document.NoChildren();
}

bool CXMLDocument::appendChild(IXMLDOMNode* pNode)
{
	NOT_IMPLEMENTED;
	return false;
}

XDOM::IXMLDOMNode* CXMLDocument::getAttribute(const XMLCHAR* sName)
{
	NOT_IMPLEMENTED;
	return nullptr;
}

XDOM::IXMLDOMNodeList* CXMLDocument::getElementsByTagName(const XMLCHAR* sName)
{
	NOT_IMPLEMENTED;
	return new CXMLDOMNodeList(m_Document.FirstChildElement(sName));
}

bool CXMLDocument::load(const XMLCHAR* sSourceFile)
{
	NOT_IMPLEMENTED;
	return m_Document.LoadFile(sSourceFile) == tinyxml2::XML_SUCCESS;
}

bool CXMLDocument::loadXML(const XMLCHAR* szString)
{
	return m_Document.Parse(szString) == tinyxml2::XML_SUCCESS;
}

XDOM::IXMLDOMNode* CXMLDocument::getRootNode()
{
	NOT_IMPLEMENTED;
	return nullptr;
}

XDOM::IXMLDOMNode* CXMLDocument::createNode(XDOM::_DOMNodeType Type, const char* name)
{
	NOT_IMPLEMENTED;
	return nullptr;
}

const XMLCHAR* CXMLDocument::getErrorString()
{
	NOT_IMPLEMENTED;
	return m_Document.ErrorStr();
}

unsigned short CXMLDocument::getCheckSum()
{
	NOT_IMPLEMENTED;
	return 0;
}
