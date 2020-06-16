
#ifndef VARIABLE_H
#define VARIABLE_H

#define SERVER_VARTYPE_NONE (0)
#define SERVER_VARTYPE_INT (1)
#define SERVER_VARTYPE_STRING (2)
#define SERVER_VARTYPE_FLOAT (3)

struct SVariableData {
	char szName[40];
	unsigned char ucType;
	union {
		cell nValue;
		char* szValue;
	};
};

typedef std::vector<struct SVariableData*> VariableVector;

class CVariables
{
private:
	VariableVector m_Variables;
public:
	CVariables();
	~CVariables();

	bool SetNumber(char* szVarName, cell nValue, bool bAsFloat);
	bool SetString(char* szVarName, char* szValue);
	cell GetNumber(char* szVarName);
	char* GetString(char* szVarName);
	bool Delete(char* szVarName);
	int GetType(char* szVarName);
	char* GetNameAtIndex(int iIndex);
	int GetUpperIndex();
};

#endif // VARIABLE_H
