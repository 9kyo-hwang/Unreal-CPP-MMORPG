#pragma once

#include "rapidxml.hpp"
using namespace rapidxml;

using FXmlNodeType = xml_node<TCHAR>;
using FXmlDocumentType = xml_document<TCHAR>;
using FXmlAttributeType = xml_attribute<TCHAR>;

class FXmlNode
{
public:
	FXmlNode(FXmlNodeType* InNode = nullptr);

	bool IsValid() const { return Node != nullptr; }

	bool			GetAttribute(const TCHAR* InTag, bool DefaultValue = false) const;
	int8			GetAttribute(const TCHAR* InTag, int8 DefaultValue = 0) const;
	int16			GetAttribute(const TCHAR* InTag, int16 DefaultValue = 0) const;
	int32			GetAttribute(const TCHAR* InTag, int32 DefaultValue = 0) const;
	int64			GetAttribute(const TCHAR* InTag, int64 DefaultValue = 0) const;
	float			GetAttribute(const TCHAR* InTag, float DefaultValue = 0.0f) const;
	double			GetAttribute(const TCHAR* InTag, double DefaultValue = 0.0) const;
	const TCHAR*	GetAttribute(const TCHAR* InTag, const TCHAR* DefaultValue = TEXT("")) const;

	bool			GetContent(bool DefaultValue = false) const;
	int8			GetContent(int8 DefaultValue = 0) const;
	int16			GetContent(int16 DefaultValue = 0) const;
	int32			GetContent(int32 DefaultValue = 0) const;
	int64			GetContent(int64 DefaultValue = 0) const;
	float			GetContent(float DefaultValue = 0) const;
	double			GetContent(double DefaultValue = 0.0) const;
	const TCHAR*	GetContent(const TCHAR* DefaultValue = TEXT("")) const;

	FXmlNode FindChildNode(const TCHAR* InTag) const;
	TArray<FXmlNode> GetChildrenNodes(const TCHAR* InTag) const;

private:
	FXmlNodeType* Node;
};

class FRapidXml
{
public:
	bool ParseXmlFile(const TCHAR* XmlFilePath, FXmlNode& Root);

private:
	shared_ptr<FXmlDocumentType> Document;
	FString Data;
};

