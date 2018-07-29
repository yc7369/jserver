/*================================================================
*   Copyright (C) 2018 Chen Yang. All rights reserved.
*   
*   文件名称：config_reader.cc
*   创 建 者：Jeson Yang
*   创建日期：2018年07月31日
*   描    述：
*
================================================================*/


#include "config_reader.h"
#include "dotconfpp.h"

ConfigReader::ConfigReader() : m_conf(0)
{
}

ConfigReader::~ConfigReader()
{
    if (m_conf)
	{
        delete m_conf;
	}
}


bool ConfigReader::SetSource(const char *file, bool ignorecase)
{
    m_conf = new DOTCONFDocument(ignorecase ?
        DOTCONFDocument::CASEINSENSETIVE :
    DOTCONFDocument::CASESENSETIVE);

    if (m_conf->setContent(file) == -1)
    {
        delete m_conf;
        m_conf = 0;
        return false;
    }

    return true;
}

std::string ConfigReader::GetValueAsString(const char* name, const char* def /* =  */)
{
    if(!m_conf)
        return std::string(def);

    DOTCONFDocumentNode *node = (DOTCONFDocumentNode *)m_conf->findNode(name);
    if(!node || !node->getValue())
        return std::string(def);

    return std::string(node->getValue());
}


bool ConfigReader::GetValueAsBool(const char* name, const bool def /* = false */)
{
    if(!m_conf)
        return false;

    DOTCONFDocumentNode *node = (DOTCONFDocumentNode *)m_conf->findNode(name);
    if(!node || !node->getValue())
        return def;

    const char* str = node->getValue();
    if(strcmp(str, "true") == 0 || strcmp(str, "TRUE") == 0 ||
        strcmp(str, "yes") == 0 || strcmp(str, "YES") == 0 ||
        strcmp(str, "1") == 0)
    {
        return true;
    }

    return false;
}

int ConfigReader::GetValueAsInt(const char* name, const int def /* = 0 */)
{
    if(!m_conf)
        return def;

    DOTCONFDocumentNode *node = (DOTCONFDocumentNode *)m_conf->findNode(name);
    if(!node || !node->getValue())
        return def;

	const char *str = node->getValue();

	int value = 0;

	if (strncmp(str, "0x", 2) == 0 || strncmp(str, "0X", 2) == 0)
	{
		value = strtol(str, NULL, 16);
	}
	else
	{
		value = strtol(str, NULL, 10);
	}

    return value;
}

unsigned int ConfigReader::GetValueAsUInt(const char *name, unsigned int def /* = 0 */)
{
	if(!m_conf)
		return def;

	DOTCONFDocumentNode *node = (DOTCONFDocumentNode *)m_conf->findNode(name);
	if(!node || !node->getValue())
		return def;

	const char *str = node->getValue();

	unsigned int value = 0;

	if (strncmp(str, "0x", 2) == 0 || strncmp(str, "0X", 2) == 0)
	{
		value = strtoul(str, NULL, 16);
	}
	else
	{
		value = strtoul(str, NULL, 10);
	}

	return value;
}


float ConfigReader::GetValueAsFloat(const char* name, float def /* = 0 */)
{
    if(!m_conf)
        return def;

    DOTCONFDocumentNode *node = (DOTCONFDocumentNode *)m_conf->findNode(name);
    if(!node || !node->getValue())
        return false;

    float value = (float)atof(node->getValue());

    return value;
}

