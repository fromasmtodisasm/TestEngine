#include <tinyxml2.h>

#include <BlackBox\System\ISystem.hpp>
#include <memory>

class CXMLDocument;
class CIXMLDOMNode;
class CXMLDOMNodeList;

#define DeclareUnique(T, F)                                   \
	class T##Unique : public std::unique_ptr<T, void (*)(T*)> \
	{                                                         \
	public:                                                   \
		using Base = T;                                       \
		using Uniq = std::unique_ptr<T, void (*)(T*)>;        \
		T##Unique(Base* ptr = nullptr)                        \
		    : Uniq(ptr, [](T* b)                              \
		           { { F(b); }; })                                       \
		{                                                     \
		}                                                     \
	};

template<typename T>
void DummyDeleter(T* p)
{
}

#if 0
using Node = tinyxml2::XMLNode;
/////////////////////////////////////////////////////
DeclareUnique(Node, (DummyDeleter));
/////////////////////////////////////////////////////
using NodeList = tinyxml2::XMLNode;
/////////////////////////////////////////////////////
DeclareUnique(NodeList, (DummyDeleter));
/////////////////////////////////////////////////////
#else
using XNode     = tinyxml2::XMLNode*;
using XNodeList = std::vector<XNode>;
#endif

class CIXMLDOMNode :
    public _reference_target_t,
    public XDOM::IXMLDOMNode
{
	// IXMLDOMBase interface
public:
	virtual int  AddRef() override;
	virtual void Release() override;

	// IXMLDOMNode interface
public:
	CIXMLDOMNode(tinyxml2::XMLNode* pNode);
	virtual XDOM::_DOMNodeType     getNodeType() override;
	virtual const char*            getText() override;
	virtual const char*            getName() override;
	virtual XDOM::IXMLDOMNodeList* getChildNodes() override;
	virtual void                   setText(const char* sText) override;
	virtual void                   setName(const char* sName) override;
	virtual bool                   hasChildNodes() override;
	virtual bool                   appendChild(IXMLDOMNode* pNode) override;
	virtual XDOM::IXMLDOMNode*     getAttribute(const char* sName) override;
	virtual XDOM::IXMLDOMNodeList* getElementsByTagName(const char* sName) override;

	XNode                          m_Node;
};

class CXMLDOMNodeList :
    public _reference_target_t,
    public XDOM::IXMLDOMNodeList
{
	// IXMLDOMBase interface
public:
	virtual int  AddRef() override;
	virtual void Release() override;

public:
	CXMLDOMNodeList(tinyxml2::XMLNode* first);

	// IXMLDOMNodeList interface
public:
	virtual size_t             length() override;
	virtual void               reset() override;
	virtual XDOM::IXMLDOMNode* nextNode() override;

	XNodeList                  m_NodeList;
	XNodeList::iterator        m_CurrentNode;
};

class CXMLDocument : public XDOM::IXMLDOMDocument, _reference_target_t
{
public:
	// Inherited via IXMLDOMDocument
	virtual int                    AddRef() override;
	virtual void                   Release() override;
	virtual XDOM::_DOMNodeType     getNodeType() override;
	virtual const char*            getText() override;
	virtual const char*            getName() override;
	virtual XDOM::IXMLDOMNodeList* getChildNodes() override;
	virtual void                   setText(const char* sText) override;
	virtual void                   setName(const char* sName) override;
	virtual bool                   hasChildNodes() override;
	virtual bool                   appendChild(IXMLDOMNode* pNode) override;
	virtual XDOM::IXMLDOMNode*     getAttribute(const XMLCHAR* sName) override;
	virtual XDOM::IXMLDOMNodeList* getElementsByTagName(const XMLCHAR* sName) override;
	virtual bool                   load(const XMLCHAR* sSourceFile) override;
	virtual bool                   loadXML(const XMLCHAR* szString) override;
	virtual XDOM::IXMLDOMNode*     getRootNode() override;
	virtual XDOM::IXMLDOMNode*     createNode(XDOM::_DOMNodeType Type, const char* name) override;
	virtual const XMLCHAR*         getErrorString() override;
	virtual unsigned short         getCheckSum() override;

	tinyxml2::XMLDocument          m_Document;
	XNode                          m_Root;
};
