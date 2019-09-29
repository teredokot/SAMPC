//----------------------------------------------------------
//
//   SA:MP Multiplayer Modification For GTA:SA
//   Copyright 2004-2006 SA:MP team
//
//----------------------------------------------------------

#ifndef _SIGNER_H_INCLUDED
#define _SIGNER_H_INCLUDED

#include "../raknet/RSACrypt.h"
#include "../raknet/SHA1.h"

#define SIGNER_RSA_BIT_SIZE big::u1024
#define SIGNER_SHA1_SIZE 20

class CSigner 
{
private:
	big::RSACrypt<SIGNER_RSA_BIT_SIZE> m_rsaCrypt;
	CSHA1 m_sha1;
	
	big::u32 m_e;
	SIGNER_RSA_BIT_SIZE m_n;
	BIGHALFSIZE(SIGNER_RSA_BIT_SIZE, m_p);
	BIGHALFSIZE(SIGNER_RSA_BIT_SIZE, m_q);

	unsigned char m_pbPubKeyBuffer[sizeof(big::u32)+sizeof(SIGNER_RSA_BIT_SIZE)];
	unsigned char m_pbSignBuffer[sizeof(SIGNER_RSA_BIT_SIZE)];

public:
	CSigner()	{};
	~CSigner()	{};

	void GenerateKey(char* szFilename = 0);
	bool ReadKeyForSigning(char* szFilename);
	
	unsigned int GetPublicKeySize() { return sizeof(m_pbPubKeyBuffer); }
    unsigned char* GetPublicKey();
	void SetPublicKey(unsigned char* pbKey);

	unsigned int GetSignatureSize() { return sizeof(SIGNER_RSA_BIT_SIZE); }
	unsigned char* GetSignature()	{ return m_pbSignBuffer; };
	void SetSignature(unsigned char* pbSignature);

	void SignHash(unsigned char* pbSHA1Hash);
	bool VerifyHash(unsigned char* pbSHA1Hash);

	void SignData(unsigned char* pbData, const unsigned int dwLength);
	void DecryptSignBuffer(unsigned char* pbData);
};


#endif

