#include "xml.h"
#include <BlackBox/System/File/CryFile.h>

inline void _not_implemented(const char* file, int line, const char* fn)
{
	CryError("NOT IMPLEMENTED: %s:%d:%s", file, line, fn);
	assert(0);
}

#define NOT_IMPLEMENTED _not_implemented(__FILE__, __LINE__, __FUNCTION__)

int CIXMLDOMNode::AddRef()
{
	_reference_target_t::AddRef();
	return NumRefs();
}

void CIXMLDOMNode::Release()
{
	_reference_target_t::Release();
}

CIXMLDOMNode::CIXMLDOMNode(tinyxml2::XMLNode* pNode)
    : m_Node(pNode)
{
}

XDOM::_DOMNodeType CIXMLDOMNode::getNodeType()
{
	NOT_IMPLEMENTED;
	return {};
}

const char* CIXMLDOMNode::getText()
{
	if (auto child = m_Node->FirstChild(); child)
	{
		return child->Value();
	}
	return nullptr;
}

const char* CIXMLDOMNode::getName()
{
	return m_Node->Value();
}

XDOM::IXMLDOMNodeList* CIXMLDOMNode::getChildNodes()
{
	return new CXMLDOMNodeList(m_Node->FirstChild());
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
	return !m_Node->NoChildren();
}

bool CIXMLDOMNode::appendChild(IXMLDOMNode* pNode)
{
	NOT_IMPLEMENTED;
	return {};
}

XDOM::IXMLDOMNode* CIXMLDOMNode::getAttribute(const char* sName)
{
	NOT_IMPLEMENTED;
	return {};
}

XDOM::IXMLDOMNodeList* CIXMLDOMNode::getElementsByTagName(const char* sName)
{
	if (auto root = m_Node->FirstChildElement(sName); root)
	{
		return new CXMLDOMNodeList(root);
	}
	return nullptr;
}

int CXMLDOMNodeList::AddRef()
{
	::_reference_target_t::AddRef();
	return NumRefs();
}

void CXMLDOMNodeList::Release()
{
	::_reference_target_t::Release();
}

CXMLDOMNodeList::CXMLDOMNodeList(tinyxml2::XMLNode* first)
{
	m_NodeList.push_back(first);
	for (; first; first = first->NextSibling())
	{
		m_NodeList.push_back(first);
	}
	m_CurrentNode = m_NodeList.begin();
}

size_t CXMLDOMNodeList::length()
{
	return m_NodeList.size();
}

void CXMLDOMNodeList::reset()
{
	m_CurrentNode = m_NodeList.begin();
}

XDOM::IXMLDOMNode* CXMLDOMNodeList::nextNode()
{
	if (m_CurrentNode == m_NodeList.end())
		return nullptr;
	return new CIXMLDOMNode(*(m_CurrentNode++));
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
	if (auto root = m_Root->FirstChildElement(sName); root)
	{
		return new CXMLDOMNodeList(root);
	}
	return nullptr;
}

bool CXMLDocument::load(const XMLCHAR* sSourceFile)
{
	if (CCryFile file(sSourceFile, "r"); file)
	{
		std::vector<char> Xml;
		Xml.reserve(file.GetLength());
		file.Read(Xml.data(), file.GetLength());
		if (m_Document.Parse(Xml.data(), file.GetLength()) == tinyxml2::XML_SUCCESS)
		{
			getRootNode();
			return m_Root != nullptr;
		}
		return false;
	}
	return false;
}

bool CXMLDocument::loadXML(const XMLCHAR* szString)
{
	return m_Document.Parse(szString) == tinyxml2::XML_SUCCESS;
}

XDOM::IXMLDOMNode* CXMLDocument::getRootNode()
{
	m_Root = m_Document.FirstChild();
	while (m_Root->NoChildren())
	{
		m_Root = m_Root->NextSibling();
	}
	return new CIXMLDOMNode(m_Root);
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
