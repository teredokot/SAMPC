
#include "main.h"

CVariables::CVariables()
{

}

CVariables::~CVariables()
{
	for (auto& v : m_Variables) {
		if (v->ucType == SERVER_VARTYPE_STRING) {
			free(v->szValue);
		}
	}
	m_Variables.clear();
}

bool CVariables::SetNumber(char* szVarName, cell nValue, bool bAsFloat)
{
	for (const auto& v : m_Variables) {
		if (strcmp(szVarName, v->szName) == 0 && v->ucType != SERVER_VARTYPE_STRING) {
			v->nValue = nValue;
			return true;
		}
	}

	SVariableData* pVariable;
	try {
		pVariable = new SVariableData;
	}
	catch (...) {
		return false;
	}

	strcpy_s(pVariable->szName, szVarName);
	pVariable->ucType = bAsFloat ? SERVER_VARTYPE_FLOAT : SERVER_VARTYPE_INT;
	pVariable->nValue = nValue;

	m_Variables.push_back(pVariable);
	return true;
}

bool CVariables::SetString(char* szVarName, char* szValue)
{
	char* szNewValue = (char*)malloc(strlen(szValue) + 1);
	if (szNewValue != NULL) {
		strcpy(szNewValue, szValue);

		for (const auto& v : m_Variables) {
			if (strcmp(szVarName, v->szName) == 0 && v->ucType == SERVER_VARTYPE_STRING) {
				free(v->szValue);
				v->szValue = szNewValue;
				return true;
			}
		}

		SVariableData* pVariable;
		try {
			pVariable = new SVariableData;
		}
		catch (...) {
			return false;
		}

		strcpy_s(pVariable->szName, szVarName);
		pVariable->ucType = SERVER_VARTYPE_STRING;
		pVariable->szValue = szNewValue;

		m_Variables.push_back(pVariable);
		return true;
	}
	return false;
}

cell CVariables::GetNumber(char* szVarName)
{
	for (const auto& v : m_Variables) {
		if (strcmp(szVarName, v->szName) == 0 && v->ucType != SERVER_VARTYPE_STRING) {
			return v->nValue;
		}
	}
	return 0;
}

char* CVariables::GetString(char* szVarName)
{
	for (const auto& v : m_Variables) {
		if (strcmp(szVarName, v->szName) == 0 && v->ucType == SERVER_VARTYPE_STRING) {
			return v->szValue;
		}
	}
	return NULL;
}

bool CVariables::Delete(char* szVarName)
{
	VariableVector::iterator it = m_Variables.begin();
	for (; it != m_Variables.end(); ++it) {
		if (strcmp(szVarName, (*it)->szName) == 0) {
			if ((*it)->ucType == SERVER_VARTYPE_STRING) {
				free((*it)->szValue);
			}
			m_Variables.erase(it);
			return true;
		}
	}
	return false;
}

int CVariables::GetType(char* szVarName)
{
	for (const auto& v : m_Variables) {
		if (strcmp(szVarName, v->szName) == 0) {
			return v->ucType;
		}
	}
	return SERVER_VARTYPE_NONE;
}

char* CVariables::GetNameAtIndex(int iIndex)
{
	try {
		if (iIndex < (int)m_Variables.size() && m_Variables[iIndex]) {
			return m_Variables[iIndex]->szName;
		}
	} catch(...) {
		return NULL;
	}
	return NULL;
}

int CVariables::GetUpperIndex()
{
	return m_Variables.size();
}
