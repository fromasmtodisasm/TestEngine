#include <tinyxml2.h>

#include <BlackBox\System\ISystem.hpp>

class CXMLDocument;
class CIXMLDOMNode;
class CXMLDOMNodeList;

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

	tinyxml2::XMLNode*         m_NodeList;
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
};
