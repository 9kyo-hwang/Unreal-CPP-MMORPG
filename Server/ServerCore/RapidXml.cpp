#include "pch.h"
#include "RapidXml.h"

#include "FileHelper.h"

_locale_t Kor = _create_locale(LC_NUMERIC, "kor");

FXmlNode::FXmlNode(FXmlNodeType* InNode)
	: Node(InNode)
{
	
}

// <Column [Attribute]name=""...> Content </Column>
bool FXmlNode::GetAttribute(const TCHAR* InTag, bool DefaultValue) const
{
	if (FXmlAttributeType* Attribute = Node->first_attribute(InTag))
	{
		return ::_wcsicmp(Attribute->value(), TEXT("true")) == 0;
	}

	return DefaultValue;
}

int8 FXmlNode::GetAttribute(const TCHAR* InTag, int8 DefaultValue) const
{
	if (FXmlAttributeType* Attribute = Node->first_attribute(InTag))
	{
		return static_cast<int8>(::_wtoi(Attribute->value()));
	}

	return DefaultValue;
}

int16 FXmlNode::GetAttribute(const TCHAR* InTag, int16 DefaultValue) const
{
	if (FXmlAttributeType* Attribute = Node->first_attribute(InTag))
	{
		return static_cast<int16>(::_wtoi(Attribute->value()));
	}

	return DefaultValue;
}

int32 FXmlNode::GetAttribute(const TCHAR* InTag, int32 DefaultValue) const
{
	if (FXmlAttributeType* Attribute = Node->first_attribute(InTag))
	{
		return ::_wtoi(Attribute->value());
	}

	return DefaultValue;
}

int64 FXmlNode::GetAttribute(const TCHAR* InTag, int64 DefaultValue) const
{
	if (FXmlAttributeType* Attribute = Node->first_attribute(InTag))
	{
		return ::_wtoi64(Attribute->value());
	}

	return DefaultValue;
}

float FXmlNode::GetAttribute(const TCHAR* InTag, float DefaultValue) const
{
	if (FXmlAttributeType* Attribute = Node->first_attribute(InTag))
	{
		return static_cast<float>(::_wtof(Attribute->value()));
	}

	return DefaultValue;
}

double FXmlNode::GetAttribute(const TCHAR* InTag, double DefaultValue) const
{
	if (FXmlAttributeType* Attribute = Node->first_attribute(InTag))
	{
		return ::_wtof(Attribute->value());
	}

	return DefaultValue;
}

const TCHAR* FXmlNode::GetAttribute(const TCHAR* InTag, const TCHAR* DefaultValue) const
{
	if (FXmlAttributeType* Attribute = Node->first_attribute(InTag))
	{
		return Attribute->value();
	}

	return DefaultValue;
}

bool FXmlNode::GetContent(bool DefaultValue) const
{
	if (TCHAR* Content = Node->value())
	{
		return ::_wcsicmp(Content, TEXT("true")) == 0;
	}

	return DefaultValue;
}

int8 FXmlNode::GetContent(int8 DefaultValue) const
{
	if (TCHAR* Content = Node->value())
	{
		return static_cast<int8>(::_wtoi(Content));
	}

	return DefaultValue;
}

int16 FXmlNode::GetContent(int16 DefaultValue) const
{
	if (TCHAR* Content = Node->value())
	{
		return static_cast<int16>(::_wtoi(Content));
	}

	return DefaultValue;
}

int32 FXmlNode::GetContent(int32 DefaultValue) const
{
	if (TCHAR* Content = Node->value())
	{
		return ::_wtoi(Content);
	}

	return DefaultValue;
}

int64 FXmlNode::GetContent(int64 DefaultValue) const
{
	if (TCHAR* Content = Node->value())
	{
		return ::_wtoi64(Content);
	}

	return DefaultValue;
}

float FXmlNode::GetContent(float DefaultValue) const
{
	if (TCHAR* Content = Node->value())
	{
		return static_cast<float>(::_wtof(Content));
	}

	return DefaultValue;
}

double FXmlNode::GetContent(double DefaultValue) const
{
	if (TCHAR* Content = Node->value())
	{
		return ::_wtof(Content);
	}

	return DefaultValue;
}

const TCHAR* FXmlNode::GetContent(const TCHAR* DefaultValue) const
{
	if (TCHAR* Content = Node->value())
	{
		return Content;
	}

	return DefaultValue;
}

FXmlNode FXmlNode::FindChildNode(const TCHAR* InTag) const
{
	return FXmlNode(Node->first_node(InTag));
}

TArray<FXmlNode> FXmlNode::GetChildrenNodes(const TCHAR* InTag) const
{
	TArray<FXmlNode> Nodes;
	FXmlNodeType* Root = Node->first_node(InTag);
	while (Root)
	{
		Nodes.push_back(FXmlNode(Root));
		Root = Root->next_sibling(InTag);
	}

	return Nodes;
}

bool FRapidXml::ParseXmlFile(const TCHAR* XmlFilePath, FXmlNode& Root)
{
	TArray<uint8> Bytes;
	FFileHelper::LoadFileToArray(Bytes, XmlFilePath);

	Data = FFileHelper::Convert(std::string(Bytes.begin(), Bytes.end()));
	if (Data.empty())
	{
		return false;
	}

	Document = MakeShared<FXmlDocumentType>();
	Document->parse<0>(&Data[0]);

	Root = FXmlNode(Document->first_node());	// <GameDB> ... </GameDB>
	return true;
}
